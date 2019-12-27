#include <avr/wdt.h>
#include "LowPower.h"
#include <AltSoftSerial.h>
#include <EEPROM.h>
#include <CayenneLPP.h>
#include <Wire.h>
#include "ClosedCube_SHT31D.h"

const uint8_t SHT_ALR_PIN   = 0;  // PD2/RXD1/INT2
const uint8_t SHT_RES_PIN   = 1;  // PD3/TXD1/INT3
const uint8_t RAK_RES_PIN   = 4;   // PD4/ADC8
const uint8_t LED_PIN       = 10;  // PB6/ADC13/PCINT6
const uint8_t BAT_PIN       = A0;  // PF7/ADC7
const uint8_t BAT_EN_PIN    = A1;  // PF6/ADC6
const uint8_t VREF_EN_PIN   = A2;  // PF5/ADC5

float BatVolt;
volatile bool isAlarm = false;
bool isPowerUp;
const uint8_t batEnDly = 1, batSampDly = 1, batSampNum = 3;
const uint8_t atWake = 1, atSleep = 2, atJoin = 3, atSend = 4, atDr = 5;
uint16_t minuteSend, sendP;
const long tmrSec120 = 120000, tmrSec10 = 10000, tmrMsec100 = 100;

struct Conf {
  uint16_t read_p;
  uint16_t send_p;
  float alr_lo_t;
  float alr_hi_t;
  float alr_hys_t;
  float alr_lo_h;
  float alr_hi_h;
  float alr_hys_h;
  uint8_t dr;      
};

Conf conf;
ClosedCube_SHT31D sht;
AltSoftSerial rakSerial;
CayenneLPP lpp(51);

void setup() {  
  setPins();
  rakSerial.begin(9600);
  resSht();
  analogReference(INTERNAL);
  loadConf();  
  flashLed();
  setSht();    
  if (USBSTA >> VBUS & 1) {
    setUsb();
  }
  pwrDownUsb();
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
      uplink();
      return;      
    }              
  }    
  minuteSend++;  
  if (minuteSend >= sendP) {
    uplink();
    return;
  }    
}
void sleepAndWake() { 
  EIFR = 255;  
  attachInterrupt(digitalPinToInterrupt(SHT_ALR_PIN), wakeUp, CHANGE);  
  isAlarm = false; 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); ///??????????????????????????????? BOD_OFF
  detachInterrupt(digitalPinToInterrupt(SHT_ALR_PIN));
}
void uplink() {
  isAlarm = false;  
  minuteSend = 0; 
  readBattery(); 
  SHT31D result = sht.periodicFetchData();  
  lpp.reset();  
  lpp.addTemperature(1, result.t);
  lpp.addRelativeHumidity(2, result.rh);
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
void setSht() {
  Wire.begin();
  sht.begin(0x44);  
  sht.periodicStart(SHT3XD_REPEATABILITY_LOW, SHT3XD_FREQUENCY_HZ5);
  sht.writeAlertHigh(conf.alr_hi_t, conf.alr_hi_t - conf.alr_hys_t, conf.alr_hi_h, conf.alr_hi_h - conf.alr_hys_h);
  sht.writeAlertLow(conf.alr_lo_t + conf.alr_hys_t, conf.alr_lo_t,  conf.alr_lo_h + conf.alr_hys_h, conf.alr_lo_h);
  SHT31D result = sht.periodicFetchData();
  sht.clearAll();     
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
    if (confKey == 2) {
      conf.send_p = confValue;       
    } else if (confKey == 3) {
      conf.alr_lo_t = confValue;      
    } else if (confKey == 4) {
      conf.alr_hi_t = confValue;      
    } else if (confKey == 5) {
      conf.alr_hys_t = confValue;      
    } else if (confKey == 6) {
      conf.alr_lo_h = confValue;
    } else if (confKey == 7) {
      conf.alr_hi_h = confValue;
    } else if (confKey == 8) {
      conf.alr_hys_h = confValue; 
      } else if (confKey == 9) {
      conf.dr = confValue;    
    }
    EEPROM.put(0, conf);    
    resetMe();  
  }  
}
void setPins() {
  pinMode(SHT_ALR_PIN, INPUT);
  pinMode(SHT_RES_PIN, OUTPUT); 
  pinMode(RAK_RES_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);  
  pinMode(BAT_PIN, INPUT);
  pinMode(BAT_EN_PIN, OUTPUT);
  pinMode(VREF_EN_PIN, OUTPUT);    
  digitalWrite(SHT_RES_PIN, LOW);
  digitalWrite(RAK_RES_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BAT_EN_PIN, HIGH);
  digitalWrite(VREF_EN_PIN, LOW);  
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
        } else if (str.startsWith(F("alr_lo_t"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_lo_t="), "");
            conf.alr_lo_t = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_lo_t);
          }              
        } else if (str.startsWith(F("alr_hi_t"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_hi_t="), "");
            conf.alr_hi_t = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_hi_t);
          }
        } else if (str.startsWith(F("alr_hys_t"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_hys_t="), "");
            conf.alr_hys_t = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_hys_t);
          }
        } else if (str.startsWith(F("alr_lo_h"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_lo_h="), "");
            conf.alr_lo_h = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_lo_h);
          }   
        } else if (str.startsWith(F("alr_hi_h"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_hi_h="), "");
            conf.alr_hi_h = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_hi_h);
          }              
        } else if (str.startsWith(F("alr_hys_h"))) {
          if (str.indexOf(F("=")) >= 0) {
            str.replace(F("alr_hys_h="), "");
            conf.alr_hys_h = str.toFloat();
            EEPROM.put(0, conf);
            Serial.println(F("OK"));
          } else {
            Serial.print(F("OK"));
            Serial.println(conf.alr_hys_h);
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
void resSht() {
  delay(100);
  digitalWrite(SHT_RES_PIN, HIGH);
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
