#include <avr/wdt.h>
#include <AltSoftSerial.h>
#include <EEPROM.h>
#include <CayenneLPP.h>
#include <Wire.h>
#include <INA226.h>

const uint8_t BUTTON_PIN    = 7;                // PE6/AIN0/INT6
const uint8_t DS_INT_PIN    = SCK;              // PB1/SCLK/PCINT1
const uint8_t INA_ALR_PIN[2]= {MOSI, MISO};     // PB2/MOSI/PCINT2, PB3/MISO/PCINT3 
const uint8_t RS_DIR_PIN    = 4;                // PD4/ADC8
const uint8_t DIG_PIN[2]    = {9, 8};           // PB5/ADC12/PCINT5, PB4/ADC11/PCINT4
const uint8_t RAK_RES_PIN   = 10;               // PB6/ADC13/PCINT6
const uint8_t RELAY_PIN[4]  = {A3, A2, A1, A0}; // (S)PF4/ADC4, (R)PF5/ADC5, (S)PF6/ADC6, (R)PF7/ADC7 
const uint8_t LED_PIN       = A5;               // PF0/ADC0

float In, Val[2];
uint8_t hysRegionPrev[2] = {3, 3};
volatile bool isAttachInt = false;
const uint8_t digDly = 50;
unsigned long tmrMillis, tmrMinutes;
String strSerial, strRakSerial;
bool loraJoin = false, loraSend = true, isValAlarm = false;

struct Conf {
  uint8_t bytes[6];   // an0_type, an1_type, dig0_type, dig1_type, dr 
  uint16_t words[1];  // send_p
  float floats[6];    // alr0_min, alr0_max, alr1_min, alr1_max, hys0, hys1
};

Conf conf;
AltSoftSerial rakSerial;
CayenneLPP lpp(51);
INA226 ina;

void setup() {
  if (USBSTA >> VBUS & 1) {
    Serial.begin(115200);
    while (!Serial);
  }
  wdt_enable(WDTO_8S);  
  setPins();
  rakSerial.begin(9600);
  loadConf();  
  setIna();
  setAttachInt();  
  resetRak(); 
  tmrMillis = millis();
}
void loop() {
  wdt_reset();
  if (millis() - tmrMillis >= conf.send_p * 60000) {
    tmrMillis = millis();
    if (loraJoin && loraSend) {
      loraSend = false;
      readAll();
      uplink();
      return;
    } else {
      resetMe();
    }    
  }
  if (isAttachInt) {
    isAttachInt = false;
    delay(digDly);
    if (loraJoin && loraSend) {
      loraSend = false;
      readAll();
      uplink();
      return;
    } else {
      resetMe();
    }          
  }
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.an_type[ch]) {
      if (!INA_ALR_PIN[ch]) {        
        readAll();        
      }
    }    
  } 
  if (isValAlarm) {
    isValAlarm = false;
    if (loraJoin && loraSend) {
      loraSend = false;
      uplink();
      return;
    } else {
      resetMe();
    }             
  }
  while (rakSerial.available()) {
    const char chrRakSerial = (char)rakSerial.read();
    //if (Serial) {
      Serial.print(chrRakSerial); // or line print
    //}
    strRakSerial += chrRakSerial;
    if (chrRakSerial == '\n') {
      strRakSerial.trim();
      if (strRakSerial.endsWith(F("Join Success"))) {        
        // delay
        rakSerial.print(F("at+set_config=lora:dr:")); 
        rakSerial.println(conf.dr);
      } else if (strRakSerial.endsWith(F("DR0 success"))) { /// DR0
        loraJoin = true; 
        digitalWrite(LED_PIN, HIGH);       
      } else if (strRakSerial.endsWith(F("send success"))) { 
        loraSend = true;
      }
      strRakSerial = "";
    }
  }
  while (Serial.available()) {
    const char chrSerial = (char)Serial.read();
    strSerial += chrSerial;
    if (chrSerial == '\n') {
      strSerial.trim();       
      if (strSerial.startsWith(F("at"))) {
        rakSerial.println(strSerial); 
      } else if (strSerial.startsWith(F("&eof"))) {
        EEPROM.put(0, conf);
        Serial.println(F("OK"));       
      } else if (strSerial.startsWith(F("&b"))) {
        conf.bytes[strSerial.substring(2,4).toInt()] = strSerial.substring(5).toInt();
      } else if (strSerial.startsWith(F("&w"))) {
        conf.words[strSerial.substring(2,4).toInt()] = strSerial.substring(5).toInt();     
      } else if (strSerial.startsWith(F("&f"))) {
        conf.floats[strSerial.substring(2,4).toInt()] = strSerial.substring(5).toFloat();
      }
      strSerial = "";
    }
  }   
}
void uplink() {
  lpp.reset();  
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.an_type[ch]) {
      lpp.addAnalogInput(ch + 1, Val[ch]);      
    } 
  } 
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.dig_type[ch]) {
      lpp.addDigitalInput(ch + 3, digitalRead(DIG_PIN[ch]));
    } 
  } 
  rakSerial.print(F("at+send=lora:1:")); 
  rakSerial.println(lppGetBuffer());  
}
void readAll() {  
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.an_type[ch]) {
      ina.begin(0x40 + ch);
      readIn();
      calcVal(ch);
      calcValAlarm(ch); 
    }
  }
}
void readIn() {   
  In = ina.readShuntCurrent();
  if (ina.isAlert()) {    
  }
}
void calcVal(const uint8_t ch) {  
  Val[ch] = (In - conf.in_min[ch]) * (conf.val_max[ch] - conf.val_min[ch]) / (conf.in_max[ch] - conf.in_min[ch]) + conf.val_min[ch];  
  //(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;  
}
void calcValAlarm(const uint8_t ch) {  
  if (Val[ch] <= conf.alr_min[ch]) {
    if (hysRegionPrev[ch] > 2) {
      isValAlarm = true;
    }
    hysRegionPrev[ch] = 1;  
  } else if ((Val[ch] >= conf.alr_min[ch] + conf.alr_hys[ch]) && (Val[ch] <= conf.alr_max[ch] - conf.alr_hys[ch])) {
    if (hysRegionPrev[ch] < 2 || hysRegionPrev[ch] > 4) {
      isValAlarm = true;
    }
    hysRegionPrev[ch] = 3; 
  } else if (Val[ch] >= conf.alr_max[ch]) {
    if (hysRegionPrev[ch] < 4) {
      isValAlarm = true;
    }
    hysRegionPrev[ch] = 5;    
  }
}
String lppGetBuffer() {
  String str;
  for(uint8_t ii = 0; ii < lpp.getSize(); ii++){    
    if (lpp.getBuffer()[ii] < 16) {
      str += '0';       
    }
    str += String(lpp.getBuffer()[ii], HEX);
    str.toUpperCase();        
  }
  return str;
}
void loadConf() {
  EEPROM.get(0, conf);    
}
void setPins() {
  pinMode(BUTTON_PIN, INPUT);  
  pinMode(DS_INT_PIN, INPUT);
  pinMode(RS_DIR_PIN, OUTPUT);
  pinMode(RAK_RES_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    pinMode(INA_ALR_PIN[ch], INPUT);
    pinMode(DIG_PIN[ch], INPUT);
  }
  for (uint8_t ch = 0; ch < 4 ; ch++) {
    pinMode(RELAY_PIN[ch], OUTPUT);    
  }   
  digitalWrite(RAK_RES_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  for (uint8_t ch = 0; ch < 4 ; ch++) {
    digitalWrite(RELAY_PIN[ch], LOW);    
  }   
}
void setIna() {
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    ina.begin(0x40 + ch);
    ina.configure(INA226_AVERAGES_128, INA226_BUS_CONV_TIME_140US, INA226_SHUNT_CONV_TIME_8244US, INA226_MODE_SHUNT_CONT);
    ina.calibrate(3.3, 0.30);
    ina.enableConversionReadyAlert();       
  } 
}
void setAttachInt() {
  EIFR = 255;  
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.dig_type[ch]) {
      attachInterrupt(digitalPinToInterrupt(DIG_PIN[ch]), attachInt, conf.dig_type[ch]);    
    }    
  }   
  isAttachInt = false;
}
void attachInt() {
  isAttachInt = true;   
}
void resetRak() {
  delay(100);
  digitalWrite(RAK_RES_PIN, HIGH);
}
void resetMe() {
  wdt_enable(WDTO_15MS);
  while(true); 
}
