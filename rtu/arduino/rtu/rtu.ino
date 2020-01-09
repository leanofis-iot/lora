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

const uint8_t amp_en_i      = 0;
const uint8_t dig_en_i      = 2;
const uint8_t dig_int_i     = 4;
const uint8_t lora_dr_i     = 6;
const uint8_t lora_port_i   = 7;
const uint8_t send_per_i    = 0;
const uint8_t alr_min_i     = 0;
const uint8_t alr_max_i     = 2;
const uint8_t alr_hys_i     = 4;
const uint8_t amp_min_i     = 6;
const uint8_t amp_max_i     = 8;
const uint8_t val_min_i     = 10;
const uint8_t val_max_i     = 12;

float Amp, Val[2];
uint8_t hysRegionPrev[2] = {3, 3};
const uint8_t digDly = 50;
volatile bool isAttachInt = false;
unsigned long tmrMillis, tmrMinutes;
String strSerial, strRakSerial;
bool loraJoin = false, loraSend = true, isValAlarm = false;

struct Conf {
  uint8_t bytes[5];   
  uint16_t words[1];  
  float floats[14];
};

Conf conf;
AltSoftSerial rakSerial;
CayenneLPP lpp(51);
INA226 ina;

void setup() {
  wdt_enable(WDTO_8S);
  if (USBSTA >> VBUS & 1) {    
    Serial.begin(115200);
    while (!Serial) {
      wdt_reset();
    }
  }    
  setPins();
  rakSerial.begin(9600);
  loadConf();  
  setIna();
  setAttachInt();  
  setRak(); 
  tmrMillis = millis();
}
void loop() {
  wdt_reset();
  getMillis();
  getAttachInt();
  getInaAlert();
  getValAlarm();
  getRakSerial();
  getSerial(); 
}
void getMillis() {
  if (millis() - tmrMillis >= conf.words[send_per_i] * 60000) {
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
}
void getAttachInt() {
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
}
void getInaAlert() {
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.bytes[amp_en_i + ch]) {
      if (!INA_ALR_PIN[ch]) {        
        readAll();
        return;        
      }
    }    
  } 
}
void getValAlarm() {
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
}
void getRakSerial() {
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
        rakSerial.println(conf.bytes[lora_dr_i]);
      } else if (strRakSerial.endsWith("DR" + String(conf.bytes[lora_dr_i]) +" success")) { 
        loraJoin = true; 
        digitalWrite(LED_PIN, HIGH);       
      } else if (strRakSerial.endsWith(F("send success"))) { 
        loraSend = true;
      }
      strRakSerial = "";
    }
  }
}
void getSerial() {
  while (Serial.available()) {
    const char chrSerial = (char)Serial.read();
    strSerial += chrSerial;
    if (chrSerial == '\n') {
      strSerial.trim();       
      if (strSerial.startsWith(F("at"))) {
        rakSerial.println(strSerial); 
      } else if (strSerial.startsWith(F("&eof"))) {
        EEPROM.put(0, conf);
        resetMe();       
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
    if (conf.bytes[amp_en_i + ch]) {
      lpp.addAnalogInput(ch + 1, Val[ch]);      
    } 
  } 
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.bytes[dig_en_i + ch]) {
      lpp.addDigitalInput(ch + 3, digitalRead(DIG_PIN[ch]));
    } 
  } 
  rakSerial.print("at+send=lora:" + String(conf.bytes[lora_port_i]) + ':'); 
  rakSerial.println(lppGetBuffer());  
}
void readAll() {  
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.bytes[amp_en_i + ch]) {
      ina.begin(0x40 + ch);
      readAmp();
      calcVal(ch);
      calcValAlarm(ch); 
    }
  }
}
void readAmp() {   
  Amp = ina.readShuntCurrent();
  if (ina.isAlert()) {    
  }
}
void calcVal(const uint8_t ch) {  
  Val[ch] = (Amp - conf.floats[amp_min_i + ch]) * (conf.floats[val_max_i + ch] - conf.floats[val_min_i + ch]) / (conf.floats[amp_max_i + ch] - conf.floats[amp_min_i + ch]) + conf.floats[val_min_i + ch];  
  //(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;  
}
void calcValAlarm(const uint8_t ch) {  
  if (Val[ch] <= conf.floats[alr_min_i + ch]) {
    if (hysRegionPrev[ch] > 2) {
      isValAlarm = true;
    }
    hysRegionPrev[ch] = 1;  
  } else if ((Val[ch] >= conf.floats[alr_min_i + ch] + conf.floats[alr_hys_i + ch]) && (Val[ch] <= conf.floats[alr_max_i + ch] - conf.floats[alr_hys_i + ch])) {
    if (hysRegionPrev[ch] < 2 || hysRegionPrev[ch] > 4) {
      isValAlarm = true;
    }
    hysRegionPrev[ch] = 3; 
  } else if (Val[ch] >= conf.floats[alr_max_i + ch]) {
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
    if (conf.bytes[amp_en_i + ch]) {
      ina.begin(0x40 + ch);
      ina.configure(INA226_AVERAGES_128, INA226_BUS_CONV_TIME_140US, INA226_SHUNT_CONV_TIME_8244US, INA226_MODE_SHUNT_CONT);
      ina.calibrate(3.3, 0.30);
      ina.enableConversionReadyAlert();
    }           
  } 
}
void setAttachInt() {
  EIFR = 255;  
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.bytes[dig_en_i + ch]) {
      attachInterrupt(digitalPinToInterrupt(DIG_PIN[ch]), attachInt, conf.bytes[dig_int_i + ch]);    
    }    
  }   
  isAttachInt = false;
}
void attachInt() {
  isAttachInt = true;   
}
void setRak() {
  delay(100);
  digitalWrite(RAK_RES_PIN, HIGH);
}
void resetMe() {
  wdt_enable(WDTO_15MS);
  while(true); 
}
