#include <avr/wdt.h>
#include <AltSoftSerial.h>
#include <EEPROM.h>
#include <CayenneLPP.h>
#include <Wire.h>
#include <INA226.h>
#include <DS3232RTC.h>
#include <WebUSB.h>

const uint8_t BUTTON_PIN        = 7;                // PE6/AIN0/INT6
const uint8_t DS_INT_PIN        = SCK;              // PB1/SCLK/PCINT1
const uint8_t INA_ALR_PIN[2]    = {MOSI, MISO};     // PB2/MOSI/PCINT2, PB3/MISO/PCINT3 
const uint8_t ACT_LED_PIN       = 11;               // PB7/PCINT7
const uint8_t RS_DIR_PIN        = 4;                // PD4/ADC8
const uint8_t DIG_PIN[2]        = {9, 8};           // PB5/ADC12/PCINT5, PB4/ADC11/PCINT4
const uint8_t RAK_RES_PIN       = 10;               // PB6/ADC13/PCINT6
const uint8_t RELAY_PIN[4]      = {A3, A2, A1, A0}; // (S)PF4/ADC4, (R)PF5/ADC5, (S)PF6/ADC6, (R)PF7/ADC7 
const uint8_t JOIN_LED_PIN      = A4;               // PF1/ADC1
const uint8_t RANDOM_PIN        = A5;               // PF0/ADC0

// lrb[]
const uint8_t lrb_dr_i          = 0;
const uint8_t lrb_port_i        = 1;
// lrw[]
const uint8_t lrw_report_i      = 0;
// tmb[]
const uint8_t tmb_alr_i         = 0;
// rlw[]
const uint8_t rlw_pulse_dur_i   = 0;
// anb[]
const uint8_t anb_alr_lo_set_i  = 0;
const uint8_t anb_alr_lo_clr_i  = 6;
const uint8_t anb_alr_hi_set_i  = 12;
const uint8_t anb_alr_hi_clr_i  = 18;
const uint8_t anb_unit_i        = 24;
// anf[]
const uint8_t anf_amp_min_i     = 0;
const uint8_t anf_amp_max_i     = 2;
const uint8_t anf_min_i         = 4;
const uint8_t anf_max_i         = 6;
const uint8_t anf_lo_set_i      = 8;
const uint8_t anf_lo_clr_i      = 10;
const uint8_t anf_hi_set_i      = 12;
const uint8_t anf_hi_clr_i      = 14;
// dgb[]
const uint8_t dgb_alr_lo_i      = 0;
const uint8_t dgb_alr_hi_i      = 6;

const uint8_t alr_an            = 1;
const uint8_t alr_dig           = 2;
const uint8_t alr_time          = 3;

const uint8_t set_relay         = 1;
const uint8_t res_relay         = 2;
const uint8_t set_pulse_relay   = 3;
const uint8_t res_pulse_relay   = 4;

const uint8_t unit_temp         = 1;
const uint8_t unit_hum          = 2;
const uint8_t unit_bar          = 3;
const uint8_t unit_lum          = 4;



float Amp, An[2];
uint8_t hysPrev[2] = {3, 3};
const uint8_t digDly = 10; // ms, max 16ms
bool dsIntPinPrev = HIGH;
volatile uint8_t alrType = 0, alrIndex = 0;
unsigned long tmrMillis, tmrMinutes;
String strSerial, strRakSerial;
bool loraJoin = false, loraSend = true;
tmElements_t tm;

struct Conf {
  uint8_t lrb[2];
  uint16_t lrw[1];
  uint8_t tmb[10];
  uint16_t rlw[2];
  uint8_t anb[26];
  float anf[16];
  uint8_t dgb[12];
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
  setINA226();
  setDigAlr();
  setDS3231M();
  //delayRandom();    
  setRak(); 
  tmrMillis = millis();
}
void loop() {
  for (uint8_t ch = 0; ch < 2; ch++) {    
    while (INA_ALR_PIN[ch]);
    readAn(ch);
    calcAnAlr(ch);    
  }  
  if (!DS_INT_PIN) {    
    if (dsIntPinPrev) {
      dsIntPinPrev = LOW;
      calcTmAlr();
    }    
  } else {
    dsIntPinPrev = HIGH;
  }  
  if (alrType) {
    doAction();  
  }    
  chkMillis();
  chkRakSerial();
  chkSerial();
  wdt_reset();      
}
void readAn(const uint8_t ch) {
  // wire.end();   
  ina.begin(0x40 + ch);
  Amp = ina.readShuntCurrent();
  if (ina.isAlert()) {}
  An[ch] = (Amp - conf.anf[anf_amp_min_i + ch]) * (conf.anf[anf_max_i + ch] - conf.anf[anf_min_i + ch]) / (conf.anf[anf_amp_max_i + ch] - conf.anf[anf_amp_min_i + ch]) + conf.anf[anf_min_i + ch];  
  //(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;  
}
void calcAnAlr(const uint8_t ch) {  
  if (An[ch] <= conf.anf[anf_lo_set_i + ch]) {
    if (hysPrev[ch] > 2) {
      alrType = alr_an;
      alrIndex = anb_alr_lo_set_i + ch * 3;      
    }
    hysPrev[ch] = 1;  
  } else if ((An[ch] >= conf.anf[anf_lo_clr_i + ch]) && (An[ch] <= conf.anf[anf_hi_clr_i + ch])) {
    if (hysPrev[ch] < 2) {
      alrType = alr_an;
      alrIndex = anb_alr_lo_clr_i + ch * 3;
    } else if (hysPrev[ch] > 4) {
      alrType = alr_an;
      alrIndex = anb_alr_hi_clr_i + ch * 3;
    }
    hysPrev[ch] = 3; 
  } else if (An[ch] >= conf.anf[anf_hi_set_i + ch]) {
    if (hysPrev[ch] < 4) {
      alrType = alr_an;
      alrIndex = anb_alr_hi_set_i + ch * 3;
    }
    hysPrev[ch] = 5;    
  }
}
void chkMillis() {
  if (millis() - tmrMillis >= conf.lrw[lrw_report_i] * 60000) {
    tmrMillis = millis();
    uplink();
  }
}
void chkRakSerial() {
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
        rakSerial.println(conf.lrb[lrb_dr_i]);
      } else if (strRakSerial.endsWith("DR" + String(conf.lrb[lrb_dr_i]) +" success")) { 
        loraJoin = true; 
        digitalWrite(JOIN_LED_PIN, HIGH);       
      } else if (strRakSerial.endsWith(F("send success"))) { 
        loraSend = true;
      }
      strRakSerial = "";
    }
  }
}
void chkSerial() {
  while (Serial.available()) {
    const char chrSerial = (char)Serial.read();
    strSerial += chrSerial;
    if (chrSerial == '\n') {
      strSerial.trim();       
      if (strSerial.startsWith(F("at"))) {
        rakSerial.println(strSerial);       
      } else if (strSerial.startsWith(F("&lrb"))) {
        conf.lrb[strSerial.substring(4,6).toInt()] = strSerial.substring(6).toInt();
      } else if (strSerial.startsWith(F("&lrw"))) {
        conf.lrw[strSerial.substring(4,6).toInt()] = strSerial.substring(6).toInt();
      } else if (strSerial.startsWith(F("&tmb"))) {
        conf.tmb[strSerial.substring(4,6).toInt()] = strSerial.substring(6).toInt();
      } else if (strSerial.startsWith(F("&rlw"))) {
        conf.rlw[strSerial.substring(4,6).toInt()] = strSerial.substring(6).toInt();
      } else if (strSerial.startsWith(F("&anb"))) {
        conf.anb[strSerial.substring(4,6).toInt()] = strSerial.substring(6).toInt();
      } else if (strSerial.startsWith(F("&anf"))) {
        conf.anf[strSerial.substring(4,6).toInt()] = strSerial.substring(6).toFloat();
      } else if (strSerial.startsWith(F("&dgb"))) {
        conf.dgb[strSerial.substring(4,6).toInt()] = strSerial.substring(6).toInt(); 
      } else if (strSerial.startsWith(F("&eof"))) {
        EEPROM.put(0, conf);
        resetMe();         
      } else if (strSerial.startsWith(F("&ss"))) {
        tm.Second = strSerial.substring(3).toInt();
      } else if (strSerial.startsWith(F("&mm"))) {
        tm.Minute = strSerial.substring(3).toInt();
      } else if (strSerial.startsWith(F("&hh"))) {
        tm.Hour = strSerial.substring(3).toInt();
      } else if (strSerial.startsWith(F("&dd"))) {
        tm.Day = strSerial.substring(3).toInt();
      } else if (strSerial.startsWith(F("&mo"))) {
        tm.Month = strSerial.substring(3).toInt();
      } else if (strSerial.startsWith(F("&yy"))) {
        tm.Year = strSerial.substring(3).toInt() - 1970;
      } else if (strSerial.startsWith(F("&tt"))) {
        RTC.write(tm);
      }
      strSerial = "";
    }
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
  pinMode(JOIN_LED_PIN, OUTPUT);
  pinMode(ACT_LED_PIN, OUTPUT);
  for (uint8_t ch = 0; ch < 2; ch++) {
    pinMode(INA_ALR_PIN[ch], INPUT);
    pinMode(DIG_PIN[ch], INPUT);
  }
  for (uint8_t ch = 0; ch < 4; ch++) {
    pinMode(RELAY_PIN[ch], OUTPUT);    
  } 
  digitalWrite(RS_DIR_PIN, LOW);  
  digitalWrite(RAK_RES_PIN, LOW);
  digitalWrite(JOIN_LED_PIN, LOW);
  digitalWrite(ACT_LED_PIN, HIGH);
  for (uint8_t ch = 0; ch < 4; ch++) {
    digitalWrite(RELAY_PIN[ch], LOW);    
  }   
}
void setINA226() {
  for (uint8_t ch = 0; ch < 2; ch++) { 
    // wire.end();   
    ina.begin(0x40 + ch);
    ina.configure(INA226_AVERAGES_128, INA226_BUS_CONV_TIME_140US, INA226_SHUNT_CONV_TIME_8244US, INA226_MODE_SHUNT_CONT);
    ina.calibrate(3.3, 0.30);
    ina.enableConversionReadyAlert();               
  } 
}
void setDigAlr() {
  EIFR = 255;     
  attachInterrupt(digitalPinToInterrupt(DIG_PIN[0]), digAlr0, CHANGE);       
  attachInterrupt(digitalPinToInterrupt(DIG_PIN[1]), digAlr1, CHANGE);
}
void digAlr0() {
  delayMicroseconds(digDly * 1000);
  if (DIG_PIN[0]) {    
    alrIndex = dgb_alr_hi_i; 
  } else {
    alrIndex = dgb_alr_lo_i;
  }
  alrType = alr_dig;
}
void digAlr1() {
  delayMicroseconds(digDly * 1000);
  if (DIG_PIN[1]) {
    alrIndex = dgb_alr_hi_i + 3; 
  } else {
    alrIndex = dgb_alr_lo_i + 3;
  }
  alrType = alr_dig;
}
void setDS3231M() {
  /*
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);
  */  
  RTC.setAlarm(ALM1_MATCH_HOURS, 0, conf.tmb[tmb_alr_i + 4], conf.tmb[tmb_alr_i + 3], 0);
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, conf.tmb[tmb_alr_i + 5 + 4], conf.tmb[tmb_alr_i + 5 + 3], 0);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.squareWave(SQWAVE_NONE);
  RTC.alarmInterrupt(ALARM_1, true);
  RTC.alarmInterrupt(ALARM_2, true);
}
void calcTmAlr() {
  if (RTC.alarm(ALARM_1)) {
    alrType = alr_time;
    alrIndex = conf.tmb[tmb_alr_i];
  } else if (RTC.alarm(ALARM_2)) {
    alrType = alr_time;
    alrIndex = conf.tmb[tmb_alr_i + 5];
  }  
}
void setRak() {
  delay(100);
  digitalWrite(RAK_RES_PIN, HIGH);
}
void doAction() {  
  for (uint8_t ch = 0; ch < 2; ch++) {
    if (alrType == alr_an) {
      actRelay(ch, conf.anb[alrIndex + ch]); 
      if (conf.anb[alrIndex + 2]) {
        uplink();  
      }    
    } else if (alrType == alr_dig) {
      actRelay(ch, conf.dgb[alrIndex + ch]);
      if (conf.dgb[alrIndex + 2]) {
        uplink();  
      } 
    } else if (alrType == alr_time) {
      actRelay(ch, conf.tmb[alrIndex + ch]);
      if (conf.tmb[alrIndex + 2]) {
        uplink();  
      } 
    }        
  }   
  alrType = 0;
  alrIndex = 0;  
}
void actRelay(const uint8_t ch, const uint8_t act) {
  if (act == set_relay) {                  
    setRelay(ch);
  } else if (act == res_relay) {           
    resRelay(ch);    
  } else if (act == set_pulse_relay) {       
    setRelay(ch); 
    delay(conf.rlw[rlw_pulse_dur_i + ch]);
    resRelay(ch);
  } else if (act == res_pulse_relay) {       
    resRelay(ch);
    delay(conf.rlw[rlw_pulse_dur_i + ch]);
    setRelay(ch); 
  }  
}
void setRelay(const uint8_t ch) {
  digitalWrite(RELAY_PIN[ch], HIGH);      
  delay(10);
  digitalWrite(RELAY_PIN[ch], LOW);
}
void resRelay(const uint8_t ch) {
  digitalWrite(RELAY_PIN[ch + 1], HIGH);  
  delay(10);
  digitalWrite(RELAY_PIN[ch + 1], LOW);
}
void uplink() {
  wdt_reset();
  if (loraJoin && loraSend) {
    loraSend = false;      
    lpp.reset();  
    for (uint8_t ch = 0; ch < 2; ch++) {
      if (conf.anb[anb_unit_i + ch] == unit_temp) {
        lpp.addTemperature(ch + 1, An[ch]);      
      } else if (conf.anb[anb_unit_i + ch] == unit_hum) {
        lpp.addRelativeHumidity(ch + 1, An[ch]);
      } else if (conf.anb[anb_unit_i + ch] == unit_bar) {
        lpp.addBarometricPressure(ch + 1, An[ch]);
      } else if (conf.anb[anb_unit_i + ch] == unit_lum) {
        lpp.addLuminosity(ch + 1, An[ch]);
      } else {
      lpp.addAnalogInput(1 + ch, An[ch]);
      }       
    } 
    for (uint8_t ch = 0; ch < 2; ch++) {      
      lpp.addDigitalInput(3 + ch, digitalRead(DIG_PIN[ch]));      
    } 
    rakSerial.print("at+send=lora:" + String(conf.lrb[lrb_port_i]) + ':'); 
    rakSerial.println(lppGetBuffer());
  } else {
    resetMe();
  }    
}
void delayRandom() {
  randomSeed(analogRead(RANDOM_PIN));
  const unsigned long rnd = random(24) * 5000;   // max 2 minutes
  tmrMillis = millis();
  while (millis() - tmrMillis < rnd) {
    wdt_reset();    
  }
}
void resetMe() {
  wdt_enable(WDTO_15MS);
  while(true); 
}
