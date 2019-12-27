#include <avr/wdt.h>
#include "LowPower.h"
#include <AltSoftSerial.h>
#include <EEPROM.h>
#include <CayenneLPP.h>
#include "ADS1118.h"
#include <SPI.h>

const uint8_t DIG_PIN[2]    = {3, 2}; // PD0/SCL/INT0, PD1/SDA/INT1
const uint8_t RAK_RES_PIN   = 4;   // PD4/ADC8
const uint8_t LED_PIN       = 10;  // PB6/ADC13/PCINT6
const uint8_t BAT_PIN       = A0;  // PF7/ADC7
const uint8_t BAT_EN_PIN    = A1;  // PF6/ADC6
const uint8_t VREF_EN_PIN   = A2;  // PF5/ADC5
const uint8_t VOUT_EN_PIN   = A3;  // PF4/ADC4
const uint8_t ADS_CS_PIN    = A4;  // PF1/ADC1

float In, R, Val[2], BatVolt;
uint8_t hysRegionPrev[2] = {3, 3};
const float rtd_coeff = 0.3851, rtd_r0 = 100, extRef = 2.5, r_ext = 2400;
volatile bool isAlarm = false;
bool isPowerUp;
const uint8_t vrefEnDly = 20, digDly = 100;
const uint8_t batEnDly = 1, batSampDly = 1, batSampNum = 3;
const uint8_t atWake = 1, atSleep = 2, atJoin = 3, atSend = 4, atDr = 5;
uint16_t minuteRead, minuteSend, sendP;
const long tmrSec120 = 120000, tmrSec10 = 10000, tmrMsec100 = 100;

struct Conf {
  uint16_t read_p;
  uint16_t send_p;        
  float alr_max[2];
  float alr_min[2];
  float alr_hys[2];
  float cal_b[2];  
  uint8_t an_type[2];
  uint8_t dig_type[2];
  uint8_t dr;  
};

Conf conf;
ADS1118 ads1118(ADS_CS_PIN);
AltSoftSerial rakSerial;
CayenneLPP lpp(51);

void setup() {  
  setPins();
  rakSerial.begin(9600);
  analogReference(INTERNAL);
  loadConf();  
  flashLed();
  setAds();    
  if (USBSTA >> VBUS & 1) {
    setUsb();
  }
  pwrDownUsb();
  readAll();
  readAll();
  if (rakJoin()) {
    if (!rakDr()) {
      resetMe();     
    }    
    if (!rakSleep()) {      
      resetMe();
    }
    delay(1000);    
    uplink();         
  } else {
    if (!rakDr()) {
      resetMe();     
    }       
    if (!rakSleep()) {          
      resetMe();
    }    
  }   
}
void loop() {  
  for (uint8_t slpCnt = 0; slpCnt < 8 ; slpCnt++) {   
    sleepAndWake();
    if (isAlarm) {
      readAll();
      delay(digDly);      
      uplink();
      return;      
    }              
  }    
  minuteRead++;
  minuteSend++;  
  if (minuteRead >= conf.read_p) {
    minuteRead = 0;    
    readAll();
    if (isAlarm) {      
      uplink();
      return;      
    }    
  }    
  if (minuteSend >= sendP) {
    uplink();
    return;
  }    
}
void sleepAndWake() { 
  EIFR = 255;  
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.dig_type[ch]) {
      attachInterrupt(digitalPinToInterrupt(DIG_PIN[ch]), wakeUp, conf.dig_type[ch]);    
    }    
  }   
  isAlarm = false; 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); ///??????????????????????????????? BOD_OFF
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.dig_type[ch]) {
      detachInterrupt(digitalPinToInterrupt(DIG_PIN[ch])); 
    }    
  }   
}
void uplink() {
  isAlarm = false;
  minuteRead = 0;  
  minuteSend = 0; 
  readBattery(); 
  lpp.reset();  
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.an_type[ch]) {
      lpp.addTemperature(ch + 1, Val[ch]);      
    } 
  } 
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.dig_type[ch]) {
      lpp.addDigitalInput(ch + 11, digitalRead(DIG_PIN[ch]));
    } 
  } 
  lpp.addAnalogInput(20, BatVolt);  
  //if (!isPowerUp) {
  //  isPowerUp = true;
  //  lpp.addAnalogOutput(30, 0);    
  //}
  if (!rakWake()) {
    resetMe();
  }
  if (!rakSend(lppGetBuffer())) {
    resetMe();
  }
  if (!rakSleep()) {
    resetMe();    
  }
}
void readAll() {  
  digitalWrite(VREF_EN_PIN, LOW);
  delay(vrefEnDly);
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    if (conf.an_type[ch]) {
      adjAds(ch);
      readIn();
      calcR();
      calcVal(ch);
      calcValAlarm(ch); 
    }
  }
  digitalWrite(VREF_EN_PIN, HIGH);  
}
void readIn() {   
  In = ads1118.getMilliVolts();   
  In /= 1000; // mV / 1000 = volt 
}
void calcR() {
  R = (In * r_ext) / (extRef - In);  
}
void calcVal(const uint8_t ch) {     
  Val[ch] = (R - rtd_r0) / rtd_coeff + conf.cal_b[ch];    
}
void calcValAlarm(const uint8_t ch) {  
  if (Val[ch] <= conf.alr_min[ch]) {
    if (hysRegionPrev[ch] > 2) {
      isAlarm = true;
    }
    hysRegionPrev[ch] = 1;  
  } else if ((Val[ch] >= conf.alr_min[ch] + conf.alr_hys[ch]) && (Val[ch] <= conf.alr_max[ch] - conf.alr_hys[ch])) {
    if (hysRegionPrev[ch] < 2 || hysRegionPrev[ch] > 4) {
      isAlarm = true;
    }
    hysRegionPrev[ch] = 3; 
  } else if (Val[ch] >= conf.alr_max[ch]) {
    if (hysRegionPrev[ch] < 4) {
      isAlarm = true;
    }
    hysRegionPrev[ch] = 5;    
  }
} 
void readBattery() {  
  digitalWrite(BAT_EN_PIN, LOW);
  delay(batEnDly);
  uint16_t samples[batSampNum];
  for (uint8_t ii = 0; ii < batSampNum; ii++) {
    samples[ii] = analogRead(BAT_PIN);
    delay(batSampDly);
  } 
  digitalWrite(BAT_EN_PIN, HIGH); 
  pwrDownAdc();
  pwrDownRef(); 
  BatVolt = 0;
  for (uint8_t jj = 0; jj < batSampNum; jj++) {
    BatVolt += samples[jj];
  }
  BatVolt /= batSampNum;   
  BatVolt = ( BatVolt / 1023 ) * 2.56 * 2;  
}
void setAds() {
  ads1118.begin();
  ads1118.setSamplingRate(ads1118.RATE_250SPS);  
  ads1118.setFullScaleRange(ads1118.FSR_0256); 
}
void adjAds(const uint8_t ch) {
  if (ch == 1) {
    ads1118.setInputSelected(ads1118.DIFF_0_1);
  } else if (ch == 0) {
    ads1118.setInputSelected(ads1118.DIFF_2_3);
  }
  delay(10);     
}
void loadConf() {
  EEPROM.get(0, conf);
  //if (conf.send_p < 15) {
  //  conf.send_p = 15;
  //} 
  sendP = conf.send_p * 0.8;   
}
void rakClear() {  
  while (rakSerial.available()) {
    const char inChar = (char)rakSerial.read();           
  }    
}
bool rakWake() {
  rakClear();
  rakSerial.println(F("at+set_config=device:sleep:0"));
  return rakResponse(atWake, tmrSec10); 
}
bool rakSleep() {
  rakClear();
  rakSerial.println(F("at+set_config=device:sleep:1"));
  return rakResponse(atSleep, tmrSec10); 
}
bool rakJoin() {
  digitalWrite(LED_PIN, LOW);
  resRak();
  return rakResponse(atJoin, tmrSec120);  
}
bool rakSend(String str) {
  digitalWrite(LED_PIN, LOW);  
  str = "at+send=lora:1:" + str;
  rakClear();   
  rakSerial.println(str);  
  return rakResponse(atSend, tmrSec10);    
}
bool rakDr() {
  rakClear();   
  rakSerial.print(F("at+set_config=lora:dr:")); 
  rakSerial.println(conf.dr); 
  return rakResponse(atDr, tmrSec10);
}
bool rakResponse(const uint8_t atCommand, const long atTmr) {
  String str;
  unsigned long startMs = millis();  
  while (millis() - startMs < atTmr) {    
    while (rakSerial.available()) {
      const char inChar = (char)rakSerial.read();
      str += inChar;
      if (inChar == '\n') {
        str.trim();
        if (atCommand == atWake) {
          if (str.equalsIgnoreCase(F("Wake up"))) {
            return true;
          }          
        } else if (atCommand == atSleep) {          
          if (str.equalsIgnoreCase(F("OK"))) {
            return true;
          }          
        } else if (atCommand == atJoin) {          
          if (str.equalsIgnoreCase(F("OK"))) {
            digitalWrite(LED_PIN, HIGH);
            return true;
          } else if (str.endsWith(F("99"))) {
            digitalWrite(LED_PIN, HIGH);
            return false;
          }                   
        } else if (atCommand == atSend) {
          if (str.equalsIgnoreCase(F("OK"))) {
            digitalWrite(LED_PIN, HIGH);
            return true;
          } else if (str.endsWith(F("86"))) {
            digitalWrite(LED_PIN, HIGH);
            return false;       
          }
        } else if (atCommand == atDr) {
          if (str.equalsIgnoreCase(F("OK"))) {            
            return true;
          }            
        }
        str = "";        
      }
    }        
  }  
  digitalWrite(LED_PIN, HIGH);  
  return false;  
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
/* 
  str = RakReadLine(tmrMsec100);
  if (str.endsWith(F("ff"))) {
    str.replace("ff", "");
    const uint8_t i = str.lastIndexOf(',');
    str = str.substring(i + 1);    
    lppDownlinkDec(str);
    EEPROM.put(0, conf);
  }
*/    
void lppDownlinkDec(String str) {
  char buf[6];
  str.substring(0, 2).toCharArray(buf, sizeof(buf));
  const uint8_t downCh = strtol(buf, NULL, 0);
  str.substring(2).toCharArray(buf, sizeof(buf));
  str = strtol(buf, NULL, 0);
  const uint8_t confKey = str.substring(str.length() - 2).toInt();
  const uint16_t confValue = str.substring(0, str.length() - 2).toInt();
  const uint8_t digOutValue = strtol(buf, NULL, 0);
  if (downCh == 30) {    
    if (confKey == 1) {
      conf.read_p = confValue;      
    } else if (confKey == 2) {
      conf.send_p = confValue;                    
    } else if (confKey == 3) {
      conf.alr_max[0] = confValue;      
    } else if (confKey == 4) {
      conf.alr_max[1] = confValue;      
    } else if (confKey == 5) {
      conf.alr_min[0] = confValue;      
    } else if (confKey == 6) {
      conf.alr_min[1] = confValue;
    } else if (confKey == 7) {
      conf.alr_hys[0] = confValue;
    } else if (confKey == 8) {
      conf.alr_hys[1] = confValue;
    } else if (confKey == 9) {
      conf.cal_b[0] = confValue;
    } else if (confKey == 10) {
      conf.cal_b[1] = confValue;
    } else if (confKey == 11) {
      conf.an_type[0] = confValue;
    } else if (confKey == 12) {
      conf.an_type[1] = confValue;         
    } else if (confKey == 13) {
      conf.dig_type[0] = confValue;      
    } else if (confKey == 14) {
      conf.dig_type[1] = confValue;
    } else if (confKey == 15) {
      conf.dr = confValue;     
    }
    EEPROM.put(0, conf);    
    resetMe();  
  }  
}
void setPins() {
  for (uint8_t ch = 0; ch < 2 ; ch++) {
    pinMode(DIG_PIN[ch], INPUT);
  }   
  pinMode(RAK_RES_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);  
  pinMode(BAT_PIN, INPUT);
  pinMode(BAT_EN_PIN, OUTPUT);
  pinMode(VREF_EN_PIN, OUTPUT);
  pinMode(VOUT_EN_PIN, OUTPUT);
  pinMode(ADS_CS_PIN, OUTPUT);  
  digitalWrite(RAK_RES_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BAT_EN_PIN, HIGH);
  digitalWrite(VREF_EN_PIN, HIGH);
  digitalWrite(VOUT_EN_PIN, LOW);
  digitalWrite(ADS_CS_PIN, HIGH);
}
void setUsb() {
  Serial.begin(115200);
  while (!Serial); 
  resRak(); 
  String str;
  while (true) {   
    if (Serial.available()) {
      const char chrUsb = (char)Serial.read();
      str += chrUsb;
      if (chrUsb == '\n') {
        str.trim();       
        if (str.startsWith(F("at"))) {
          rakClear();       
          rakSerial.println(str);        
        } else if (str.startsWith(F("read_p"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("read_p="), "");
            conf.read_p = str.toInt();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.read_p);
          } 
        } else if (str.startsWith(F("send_p"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("send_p="), "");
            conf.send_p = str.toInt();            
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.send_p);
          }         
        } else if (str.startsWith(F("alr_max_1"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_max_1="), "");
            conf.alr_max[0] = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_max[0]);
          }
        } else if (str.startsWith(F("alr_max_2"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_max_2="), "");
            conf.alr_max[1] = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_max[1]);
          } 
        } else if (str.startsWith(F("alr_min_1"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_min_1="), "");
            conf.alr_min[0] = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_min[0]);
          }
        } else if (str.startsWith(F("alr_min_2"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_min_2="), "");
            conf.alr_min[1] = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_min[1]);
          }
        } else if (str.startsWith(F("alr_hys_1"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_hys_1="), "");
            conf.alr_hys[0] = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_hys[0]);
          } 
        } else if (str.startsWith(F("alr_hys_2"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_hys_2="), "");
            conf.alr_hys[1] = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_hys[1]);
          }
        } else if (str.startsWith(F("cal_b_1"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("cal_b_1="), "");
            conf.cal_b[0] = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.cal_b[0]);
          }
        } else if (str.startsWith(F("cal_b_2"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("cal_b_2="), "");
            conf.cal_b[1] = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.cal_b[1]);
          }
        } else if (str.startsWith(F("an_type_1"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("an_type_1="), "");
            conf.an_type[0] = str.toInt();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.an_type[0]);
          }   
        } else if (str.startsWith(F("an_type_2"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("an_type_2="), "");
            conf.an_type[1] = str.toInt();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.an_type[1]);
          }        
        } else if (str.startsWith(F("dig_type_1"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("dig_type_1="), "");
            conf.dig_type[0] = str.toInt();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.dig_type[0]);
          }                  
        } else if (str.startsWith(F("dig_type_2"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("dig_type_2="), "");
            conf.dig_type[1] = str.toInt();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.dig_type[1]);
          }
        } else if (str.startsWith(F("dr"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("dr="), "");
            conf.dr = str.toInt();            
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.dr);
          }                            
        }
        str = "";        
      }      
    }
    if (rakSerial.available()) {
      const char chrSerial = (char)rakSerial.read();   
      Serial.print(chrSerial);   
    }      
  }    
}
void flashLed() {
  for (uint8_t ii = 0; ii < 5; ii++) {  
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
  }
}
void resRak() {
  delay(100);
  digitalWrite(RAK_RES_PIN, HIGH);
}
void pwrDownUsb() {
  USBDevice.detach();
  USBCON |= _BV(FRZCLK);  //freeze USB clock
  PLLCSR &= ~_BV(PLLE);   // turn off USB PLL
  USBCON &= ~_BV(USBE);   // disable USB
  USBCON &= ~_BV(OTGPADE);
  USBCON &= ~_BV(VBUSTE);
  UHWCON &= ~_BV(UVREGE);
}
void pwrDownAdc() {
  ADCSRA &= ~_BV(ADEN);
}
void pwrDownRef() {
  ACSR &= ~_BV(ACIE);
  ACSR |= _BV(ACD);
}
void wakeUp() {
  isAlarm = true;   
}
void resetMe() {
  wdt_enable(WDTO_15MS);
  while(true); 
}
