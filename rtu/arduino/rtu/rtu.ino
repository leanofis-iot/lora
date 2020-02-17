#include <avr/wdt.h>
#include <AltSoftSerial.h>
#include <EEPROM.h>
#include <CayenneLPP.h>
#include <Wire.h>
#include <INA226.h>
#include <DS3232RTC.h>
#include <WebUSB.h>
#include <ModbusMaster.h>

const uint8_t BUTTON_PIN          = 7;                // PE6/AIN0/INT6
const uint8_t DS_INT_PIN          = SCK;              // PB1/SCLK/PCINT1
const uint8_t INA_ALR_PIN[2]      = {MOSI, MISO};     // PB2/MOSI/PCINT2, PB3/MISO/PCINT3 
const uint8_t ACT_LED_PIN         = 11;               // PB7/PCINT7
const uint8_t RS_DIR_PIN          = 4;                // PD4/ADC8
const uint8_t DIG_PIN[2]          = {9, 8};           // PB5/ADC12/PCINT5, PB4/ADC11/PCINT4
const uint8_t RAK_RES_PIN         = 10;               // PB6/ADC13/PCINT6
const uint8_t RELAY_PIN[4]        = {A3, A2, A1, A0}; // (S)PF4/ADC4, (R)PF5/ADC5, (S)PF6/ADC6, (R)PF7/ADC7 
const uint8_t JOIN_LED_PIN        = A4;               // PF1/ADC1
const uint8_t RANDOM_PIN          = A5;               // PF0/ADC0

// uint8_t ge_u08[] 
const uint8_t ge_u08_lora_dr      = 0;  // select
const uint8_t ge_u08_lora_port    = 1;  // input
const uint8_t ge_u08_poll         = 2;  // input
const uint8_t ge_u08_mod_data_bit = 3;  // select
const uint8_t ge_u08_mod_stop_bit = 4;  // select
const uint8_t ge_u08_mod_par_bit  = 5;  // select
// uint16_t ge_u16[]
const uint8_t ge_u16_report       = 0;  // input
// uint32_t ge_u32[]
const uint8_t ge_u32_mod_baud     = 0;  // select 

// uint8_t an_u08[]
const uint8_t an_u08_enable       = 0;  // checkbox
const uint8_t an_u08_unit         = 1;  // select
const uint8_t an_u08_low_relay_1  = 2;  // select
const uint8_t an_u08_low_relay_2  = 3;  // select 
const uint8_t an_u08_low_report   = 4;  // checkbox
const uint8_t an_u08_high_relay_1 = 5;  // select
const uint8_t an_u08_high_relay_2 = 6;  // select 
const uint8_t an_u08_high_report  = 7;  // checkbox
const uint8_t an_u08_within_report= 8;  // checkbox
// float an_f32[]
const uint8_t an_f32_in_min       = 0;  // input
const uint8_t an_f32_in_max       = 1;  // input
const uint8_t an_f32_out_min      = 2;  // input
const uint8_t an_f32_out_max      = 3;  // input
const uint8_t an_f32_low          = 4;  // input
const uint8_t an_f32_high         = 5;  // input

// uint8_t dg_u08[]
const uint8_t dg_u08_enable       = 0;  // checkbox
const uint8_t dg_u08_unit         = 1;  // select
const uint8_t dg_u08_low_relay_1  = 2;  // select
const uint8_t dg_u08_low_relay_2  = 3;  // select 
const uint8_t dg_u08_low_report   = 4;  // checkbox
const uint8_t dg_u08_high_relay_1 = 5;  // select
const uint8_t dg_u08_high_relay_2 = 6;  // select 
const uint8_t dg_u08_high_report  = 7;  // checkbox
// uint16_t dg_u16[]
const uint8_t dg_u16_debounce     = 0;  // input

// uint8_t mo_u08[]
const uint8_t mo_u08_enable       = 0;  // checkbox
const uint8_t mo_u08_unit         = 1;  // select
const uint8_t mo_u08_slave        = 2;  // input
const uint8_t mo_u08_function     = 3;  // select
const uint8_t mo_u08_type         = 4;  // select
const uint8_t mo_u08_decimal      = 5;  // input
const uint8_t mo_u08_low_relay_1  = 6;  // select
const uint8_t mo_u08_low_relay_2  = 7;  // select 
const uint8_t mo_u08_low_report   = 8;  // checkbox
const uint8_t mo_u08_high_relay_1 = 9;  // select
const uint8_t mo_u08_high_relay_2 = 10; // select 
const uint8_t mo_u08_high_report  = 11; // checkbox
const uint8_t mo_u08_within_report= 12; // checkbox
// uint16_t mo_u16[]
const uint8_t mo_u16_register     = 0;  // input
// uint16_t mo_f32[]
const uint8_t mo_f32_low          = 0;  // input
const uint8_t mo_f32_high         = 1;  // input

// uint8_t tm_u08[]
const uint8_t tm_u08_enable       = 0;  // checkbox
const uint8_t tm_u08_hour         = 1;  // input
const uint8_t tm_u08_minute       = 2;  // input
const uint8_t tm_u08_time_relay_1 = 3;  // select
const uint8_t tm_u08_time_relay_2 = 4;  // select

const uint8_t numAn               = 2;
const uint8_t numDg               = 2;
const uint8_t numMo               = 8;
const uint8_t numTm               = 2;

struct Conf {
  uint8_t   ge_u08[6];
  uint16_t  ge_u16[1];
  uint32_t  ge_u32[1];
  uint8_t   an_u08[9 * numAn];
  float     an_f32[6 * numAn];
  uint8_t   dg_u08[8 * numDg];
  uint16_t  dg_u16[1 * numDg];
  uint8_t   mo_u08[13 * numMo];
  uint16_t  mo_u16[1 * numMo];
  uint16_t  mo_f32[2 * numMo];
  uint8_t   tm_u08[5 * numTm];     
};
Conf conf;

float       an[numAn];
uint8_t     dg[numDg];
float       mo[numMo];

uint8_t     an_prev[numAn];
uint16_t    mo_prev[numAn];

const uint8_t _u16                = 0;
const uint8_t _i16                = 1;

const uint8_t within              = 1;
const uint8_t low                 = 2;  
const uint8_t high                = 3;

const uint8_t change              = 4;

const uint8_t coil                = 1;
const uint8_t discrete            = 2;
const uint8_t holding             = 3;
const uint8_t input               = 4;

const uint8_t activate            = 1;  
const uint8_t deactivate          = 2;  

unsigned long tmrPoll, tmrReport, tmrRandom;
String strUsbSerial, strRakSerial;
bool loraJoin = false, loraSend = true, isReportIftt;
tmElements_t tm;

AltSoftSerial rakSerial;
CayenneLPP lpp(51);
INA226 ina;
WebUSB WebUSBSerial(1 /* https:// */, "leanofis-iot.github.io/lora");
#define usbSerial WebUSBSerial
ModbusMaster modbus;

void setup() {
  wdt_enable(WDTO_8S);
  setUsb();
  setPin();  
  loadConf();    
  setAnalog();
  setDigital();
  setModbus();
  setTm();
  //delayRandom();    
  setRak(); 
  tmrPoll = millis();
  tmrReport = millis();
}
void loop() {
  if (isPollInterval()) {
    readAnalog();
    readModbus();
  }  
  readDigital();
  readTime();
  if (isReportIftt) {
    report();  
  }
  if (isReportInterval()) {
    report();    
  }  
  readRakSerial();
  readUsbSerial();
  wdt_reset();
}
void readAnalog() {
  // wire.end();    
  for (uint8_t ch = 0; ch < numAn; ch++) {
    const uint8_t _enable = an_u08_enable + ch * sizeof(conf.an_u08) / numAn; 
    if (conf.an_u08[_enable]) {   
      while (INA_ALR_PIN[ch]);
      ina.begin(0x40 + ch);
      an[ch] = ina.readShuntCurrent();
      if (ina.isAlert());      
      const uint8_t _in_min = an_f32_in_min + ch * sizeof(conf.an_f32) / numAn;
      const uint8_t _in_max = an_f32_in_max + ch * sizeof(conf.an_f32) / numAn;
      const uint8_t _out_min = an_f32_out_min + ch * sizeof(conf.an_f32) / numAn;
      const uint8_t _out_max = an_f32_out_max + ch * sizeof(conf.an_f32) / numAn;
      an[ch] = (an[ch] - conf.an_f32[_in_min]) * (conf.an_f32[_out_max] - conf.an_f32[_out_min]) / (conf.an_f32[_in_max] - conf.an_f32[_in_min]) + conf.an_f32[_out_min];      
      isAnalogIftt(ch);      
    }                    
  }    
}
void isAnalogIftt(const uint8_t ch) {       
  const uint8_t _low = an_f32_low + ch * sizeof(conf.an_f32) / numAn; 
  const uint8_t _high = an_f32_high + ch * sizeof(conf.an_f32) / numAn;  
  uint8_t iftt; 
  if (an[ch] > conf.an_f32[_low] && an[ch] < conf.an_f32[_high]) {      
    if (an_prev[ch] != within) {
      iftt = within;      
    }              
  } else if (an[ch] <= conf.an_f32[_low]) {    
    if (an_prev[ch] != low) {
      iftt = low;      
    }              
  } else if (an[ch] >= conf.an_f32[_high]) {     
    if (an_prev[ch] != high) {
      iftt = high;      
    }
  }
  an_prev[ch] = iftt;
  if (iftt == within) {
    for (uint8_t r = 0; r < 2; r++) {        
      doRelay(r, deactivate);      
    }
    const uint8_t _report = an_u08_within_report + ch * sizeof(conf.an_u08) / numAn;
    if (conf.an_u08[_report]) {
      isReportIftt = true;
    }        
  } else if (iftt == low) {
    for (uint8_t r = 0; r < 2; r++) {
      uint8_t _relay;        
      _relay  = an_u08_low_relay_1 + ch * sizeof(conf.an_u08) / numAn;
      doRelay(r, conf.an_u08[_relay + r]);                      
    }  
    const uint8_t _report = an_u08_low_report + ch * sizeof(conf.an_u08) / numAn;
    if (conf.an_u08[_report]) {
      isReportIftt = true;
    }    
  } else if (iftt == high) {
    for (uint8_t r = 0; r < 2; r++) {
      uint8_t _relay;        
      _relay  = an_u08_high_relay_1 + ch * sizeof(conf.an_u08) / numAn;
      doRelay(r, conf.an_u08[_relay + r]);                      
    }      
    const uint8_t _report = an_u08_high_report + ch * sizeof(conf.an_u08) / numAn;
    if (conf.an_u08[_report]) {
      isReportIftt = true;
    }    
  }  
}
void readDigital() {
  for (uint8_t ch = 0; ch < numDg; ch++) {
    const uint8_t _enable = dg_u08_enable + ch * sizeof(conf.dg_u08) / numDg;
    if (conf.dg_u08[_enable]) {      
      isDigitalIftt(ch);          
    }    
  }
}
void isDigitalIftt(const uint8_t ch) {
  if (dg[ch] == change) {
    dg[ch] = digitalRead(DIG_PIN[ch]);
    const uint8_t _debounce = dg_u16_debounce + ch * sizeof(conf.dg_u16) / numDg;
    delay(conf.dg_u16[_debounce]); 
    if (dg[ch] == digitalRead(DIG_PIN[ch])) {      
      if (!dg[ch]) {        
        for (uint8_t r = 0; r < 2; r++) {
          uint8_t _relay;        
          _relay  = dg_u08_low_relay_1 + ch * sizeof(conf.dg_u08) / numDg;
          doRelay(r, conf.dg_u08[_relay + r]);                      
        }      
        const uint8_t _report = dg_u08_low_report + ch * sizeof(conf.dg_u08) / numDg;
        if (conf.dg_u08[_report]) {
          isReportIftt = true;
        }        
      } else {        
        for (uint8_t r = 0; r < 2; r++) {
          uint8_t _relay;        
          _relay  = dg_u08_high_relay_1 + ch * sizeof(conf.dg_u08) / numDg;
          doRelay(r, conf.dg_u08[_relay + r]);                      
        }      
        const uint8_t _report = dg_u08_high_report + ch * sizeof(conf.dg_u08) / numDg;
        if (conf.dg_u08[_report]) {
          isReportIftt = true;
        }        
      }      
    }                      
  }
  dg[ch] = digitalRead(DIG_PIN[ch]);       
}
void readModbus() { 
  for (uint8_t ch = 0; ch < numMo; ch++) {
    const uint8_t _enable = mo_u08_enable + ch * sizeof(conf.mo_u08) / numMo;        
    if (conf.mo_u08[_enable]) { 
      const uint8_t _slave = mo_u08_slave + ch * sizeof(conf.mo_u08) / numMo;     
      modbus.begin(conf.mo_u08[_slave], Serial1);
      const uint8_t _type = mo_u08_type + ch * sizeof(conf.mo_u08) / numMo;
      const uint8_t _function = mo_u08_function + ch * sizeof(conf.mo_u08) / numMo;
      const uint8_t _register = mo_u16_register + ch * sizeof(conf.mo_u16) / numMo;
      const uint8_t _decimal = mo_u08_decimal + ch * sizeof(conf.mo_u08) / numMo;     
      if (conf.mo_u08[_function] == coil) {
        mo[ch] = (uint16_t)(modbus.readCoils(conf.mo_u16[_register], 1));
      } else if (conf.mo_u08[_function] == discrete) {
        mo[ch] = (uint16_t)(modbus.readDiscreteInputs(conf.mo_u16[_register], 1));
      } else if (conf.mo_u08[_function] == holding) {
        if (conf.mo_u08[_type] == _u16) {
          mo[ch] = (uint16_t)(modbus.readHoldingRegisters(conf.mo_u16[_register], 1));
        } else if (conf.mo_u08[_type] == _i16) {
          mo[ch] = (int16_t)(modbus.readHoldingRegisters(conf.mo_u16[_register], 1));
        }                
      } else if (conf.mo_u08[_function] == input) {
        if (conf.mo_u08[_type] == _u16) {
          mo[ch] = (uint16_t)(modbus.readInputRegisters(conf.mo_u16[_register], 1));
        } else if (conf.mo_u08[_type] == _i16) {
          mo[ch] = (int16_t)(modbus.readInputRegisters(conf.mo_u16[_register], 1));
        }       
      } 
      mo[ch] /= pow(10, conf.mo_u08[_decimal]);     
      isModbusIftt(ch);          
    }    
  }  
}
void isModbusIftt(const uint8_t ch) {   
  const uint8_t _low = mo_f32_low + ch * sizeof(conf.mo_f32) / numMo;
  const uint8_t _high = mo_f32_high + ch * sizeof(conf.mo_f32) / numMo;
  uint8_t iftt;
  const uint8_t _function = mo_u08_function + ch * sizeof(conf.mo_u08) / numMo;    
  if (conf.mo_u08[_function] == coil || conf.mo_u08[_function] == discrete) {
    const uint16_t low_set = conf.mo_f32[_low];
    const uint16_t high_set = conf.mo_f32[_high];
    const uint16_t val = mo[ch];
    const uint16_t prev = mo_prev[ch];
    for (uint8_t i = 0; i < 16; i++) {
      if (bitRead(low_set, i)) {
        if (!bitRead(val, i)) {
          if (bitRead(prev, i)) {
            iftt = low;
          }
        }
      }
      if (bitRead(high_set, i)) {
        if (bitRead(val, i)) {
          if (!bitRead(prev, i)) {
            iftt = high;
          }
        }
      }      
      bitWrite(mo_prev[ch], i, bitRead(val, i));    
    }
  } else if (conf.mo_u08[_function] == holding || conf.mo_u08[_function] == input) {    
    if (mo[ch] > conf.mo_f32[_low] && mo[ch] < conf.mo_f32[_high]){       
      if (mo_prev[ch] != within) {
        iftt = within;        
      }              
    } else if (mo[ch] <= conf.mo_f32[_low]) {      
      if (mo_prev[ch] != low) {
        iftt = low;        
      }              
    } else if (mo[ch] >= conf.mo_f32[_high]) {       
      if (mo_prev[ch] != high) {
        iftt = high;          
      }
    }
    mo_prev[ch] = iftt;
  } 
  if (iftt == within) {
    for (uint8_t r = 0; r < 2; r++) {        
      doRelay(r, deactivate);      
    }
    const uint8_t _report = mo_u08_within_report + ch * sizeof(conf.mo_u08) / numMo;
    if (conf.mo_u08[_report]) {
      isReportIftt = true;
    }        
  } else if (iftt == low) {
    for (uint8_t r = 0; r < 2; r++) {
      uint8_t _relay;        
      _relay  = mo_u08_low_relay_1 + ch * sizeof(conf.mo_u08) / numMo;
      doRelay(r, conf.mo_u08[_relay + r]);                      
    }  
    const uint8_t _report = mo_u08_low_report + ch * sizeof(conf.mo_u08) / numMo;
    if (conf.mo_u08[_report]) {
      isReportIftt = true;
    }    
  } else if (iftt == high) {
    for (uint8_t r = 0; r < 2; r++) {
      uint8_t _relay;        
      _relay  = mo_u08_high_relay_1 + ch * sizeof(conf.mo_u08) / numMo;
      doRelay(r, conf.mo_u08[_relay + r]);                      
    }      
    const uint8_t _report = mo_u08_high_report + ch * sizeof(conf.mo_u08) / numMo;
    if (conf.mo_u08[_report]) {
      isReportIftt = true;
    }    
  }
}
void readTime() {
  for (uint8_t ch = 0; ch < numTm; ch++) {
    const uint8_t _enable = tm_u08_enable + ch * sizeof(conf.tm_u08) / numTm;
    if (conf.tm_u08[_enable]) {
      isTmIftt(ch);      
    }      
  }
}
void isTmIftt(const uint8_t ch) {
  if (!DS_INT_PIN) {
    for (uint8_t r = 0; r < 2; r++) {
      uint8_t _relay;
      if (ch == 0 && RTC.alarm(ALARM_1)) {          
        _relay  = tm_u08_time_relay_1;                             
      } else if (ch == 1 && RTC.alarm(ALARM_2)) {
        _relay  = tm_u08_time_relay_1 + sizeof(conf.tm_u08) / numTm;              
      } else {
        return;
      } 
      RTC.alarm(ALARM_1);
      RTC.alarm(ALARM_2); 
      doRelay(r, conf.tm_u08[_relay + r]);          
    }
  } 
}
bool isPollInterval() {
  const uint8_t _poll = ge_u08_poll;
  if (millis() / 1000 - tmrPoll >= conf.ge_u08[_poll]) {
    tmrPoll = millis();
    return true;
  }
}
bool isReportInterval() {
  const uint8_t _report = ge_u16_report;
  if (millis() / 60000 - tmrReport >= conf.ge_u16[_report]) {
    tmrReport = millis();
    return true;
  }
}
void readRakSerial() { 
  while (rakSerial.available()) {
    const char chr = (char)rakSerial.read();    
    strRakSerial += chr;
    if (chr == '\n') {
      strRakSerial.trim();
      if (strRakSerial.endsWith(F("Join Success"))) {        
        // delay
        rakSerial.print(F("at+set_config=lora:dr:")); 
        rakSerial.println(conf.ge_u08[ge_u08_lora_dr]);
      } else if (strRakSerial.endsWith("DR" + String(conf.ge_u08[ge_u08_lora_dr]) +" success")) { 
        loraJoin = true; 
        digitalWrite(JOIN_LED_PIN, HIGH);       
      } else if (strRakSerial.endsWith(F("send success"))) { 
        loraSend = true;
      }
      //if (usbSerial) {
      usbSerial.println(strRakSerial); 
      //}
      strRakSerial = "";
    }
  }
}
void readUsbSerial() {
  while (usbSerial && usbSerial.available()) {
    const char chr = (char)usbSerial.read();
    strUsbSerial += chr;
    if (chr == '\n') {
      strUsbSerial.trim();
      const uint8_t num = strUsbSerial.substring(7,9).toInt();
      const int16_t valInt = strUsbSerial.substring(9).toInt();
      const float valFloat = strUsbSerial.substring(9).toFloat();       
      if (strUsbSerial.startsWith(F("at"))) {
        rakSerial.println(strUsbSerial);
      } else if (strUsbSerial.startsWith(F("xge_u08"))) {
        conf.ge_u08[num] = (uint8_t)valInt;
      } else if (strUsbSerial.startsWith(F("xge_u16"))) {
        conf.ge_u16[num] = (uint16_t)valInt;
      } else if (strUsbSerial.startsWith(F("xge_u32"))) {
        conf.ge_u32[num] = (uint32_t)valInt;
      } else if (strUsbSerial.startsWith(F("xan_u08"))) {
        conf.an_u08[num] = (uint8_t)valInt;
      } else if (strUsbSerial.startsWith(F("xan_f32"))) {
        conf.an_f32[num] = valFloat;
      } else if (strUsbSerial.startsWith(F("xmo_u08"))) {
        conf.mo_u08[num] = (uint8_t)valInt;
      } else if (strUsbSerial.startsWith(F("xmo_u16"))) {
        conf.mo_u16[num] = (uint16_t)valInt;
      } else if (strUsbSerial.startsWith(F("xmo_f32"))) {
        conf.mo_f32[num] = valFloat;
      } else if (strUsbSerial.startsWith(F("xtm_u08"))) {
        conf.tm_u08[num] = (uint8_t)valInt;      
      } else if (strUsbSerial.startsWith(F("xsave"))) {
        EEPROM.put(0, conf);
        resetMe(); 
      } else if (strUsbSerial.startsWith(F("xget_ge"))) {
        getGeneral();
      } else if (strUsbSerial.startsWith(F("xget_tm"))) {
        getTm();
      } else if (strUsbSerial.startsWith(F("xget_ch"))) {
        getChannels();
      } else if (strUsbSerial.startsWith(F("xfetch"))) {
        fetchChannels();        
      } else if (strUsbSerial.startsWith(F("xss"))) {
        tm.Second = strUsbSerial.substring(3).toInt();
      } else if (strUsbSerial.startsWith(F("xmm"))) {
        tm.Minute = strUsbSerial.substring(3).toInt();
      } else if (strUsbSerial.startsWith(F("xhh"))) {
        tm.Hour = strUsbSerial.substring(3).toInt();
      } else if (strUsbSerial.startsWith(F("xdd"))) {
        tm.Day = strUsbSerial.substring(3).toInt();
      } else if (strUsbSerial.startsWith(F("xwd"))) {
        tm.Wday = strUsbSerial.substring(3).toInt();
      } else if (strUsbSerial.startsWith(F("xmh"))) {
        tm.Month = strUsbSerial.substring(3).toInt();
      } else if (strUsbSerial.startsWith(F("xyy"))) {
        tm.Year = strUsbSerial.substring(3).toInt() - 1970;
      } else if (strUsbSerial.startsWith(F("xtime"))) {
        RTC.write(tm);
      }
      strUsbSerial = "";
    }
  }   
}
void getGeneral() {
  String str; 
  for (uint8_t i = 0; i < sizeof(conf.ge_u08); i++) {    
    usbSerial.print(F("xge_u08"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.ge_u08[i]);    
  }
  for (uint8_t i = 0; i < sizeof(conf.ge_u16); i++) {
    usbSerial.print(F("xge_u16"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.ge_u16[i]);    
  }
  for (uint8_t i = 0; i < sizeof(conf.ge_u32); i++) {
    usbSerial.print(F("xge_u32"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.ge_u32[i]);    
  }  
}
void getTm() {
  String str;
  for (uint8_t i = 0; i < sizeof(conf.tm_u08); i++) {
    usbSerial.print(F("xtm_u08"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.tm_u08[i]);    
  }   
}
void getChannels() { 
  String str;  
  for (uint8_t i = 0; i < sizeof(conf.an_u08); i++) {
    usbSerial.print(F("xan_u08"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.an_u08[i]);    
  }
  for (uint8_t i = 0; i < sizeof(conf.an_f32); i++) {
    usbSerial.print(F("xan_f32"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.an_f32[i]);    
  }
  for (uint8_t i = 0; i < sizeof(conf.dg_u08); i++) {
    usbSerial.print(F("xdg_u08"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.dg_u08[i]);    
  }
  for (uint8_t i = 0; i < sizeof(conf.dg_u16); i++) {
    usbSerial.print(F("xdg_u16"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.dg_u16[i]);    
  }
  for (uint8_t i = 0; i < sizeof(conf.mo_u08); i++) {
    usbSerial.print(F("xmo_u08"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.mo_u08[i]);    
  }
  for (uint8_t i = 0; i < sizeof(conf.mo_u16); i++) {
    usbSerial.print(F("xmo_u16"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.mo_u16[i]);    
  }
  for (uint8_t i = 0; i < sizeof(conf.mo_f32); i++) {
    usbSerial.print(F("xmo_f32"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(conf.mo_f32[i]);    
  }  
}
void fetchChannels() { 
  String str;
  for (uint8_t i = 0; i < numAn; i++) {
    usbSerial.print(F("xan_val"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(an[i]);    
  }
  for (uint8_t i = 0; i < numDg; i++) {
    usbSerial.print(F("xdg_val"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(dg[i]);    
  }
  for (uint8_t i = 0; i < numMo; i++) {
    usbSerial.print(F("xmo_val"));
    str = '0' + i;
    usbSerial.print(str.substring(str.length() - 2));
    usbSerial.println(mo[i]);    
  }
}
void loadConf() {
  EEPROM.get(0, conf);  
}
void setPin() {
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
void setAnalog() {
  for (uint8_t ch = 0; ch < numAn; ch++) { 
    // wire.end();   
    ina.begin(0x40 + ch);
    ina.configure(INA226_AVERAGES_128, INA226_BUS_CONV_TIME_140US, INA226_SHUNT_CONV_TIME_8244US, INA226_MODE_SHUNT_CONT);
    ina.calibrate(3.3, 0.30);
    ina.enableConversionReadyAlert();               
  } 
}
void setDigital() {
  EIFR = 255;     
  attachInterrupt(digitalPinToInterrupt(DIG_PIN[0]), digChange0, CHANGE);       
  attachInterrupt(digitalPinToInterrupt(DIG_PIN[1]), digChange1, CHANGE);
}
void digChange0() {
  dg[0] = change;  
}
void digChange1() {
  dg[1] = change;
}
void setModbus() {
  const uint32_t _baud = conf.ge_u32[ge_u32_mod_baud];
  const uint8_t _config = conf.ge_u08[ge_u08_mod_data_bit] | conf.ge_u08[ge_u08_mod_par_bit] | conf.ge_u08[ge_u08_mod_stop_bit];    
  Serial1.begin(_baud, _config); 
  modbus.preTransmission(modbusDe);
  modbus.postTransmission(modbusRe);
}
void modbusDe() {
  digitalWrite(RS_DIR_PIN, HIGH);
}
void modbusRe() {
  digitalWrite(RS_DIR_PIN, LOW);
}
void setTm() {
  /*
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);
  */ 
  RTC.setAlarm(ALM1_MATCH_HOURS, 0, conf.tm_u08[tm_u08_minute], conf.tm_u08[tm_u08_hour], 0);
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, conf.tm_u08[tm_u08_minute + sizeof(conf.tm_u08) / numTm], conf.tm_u08[tm_u08_hour + sizeof(conf.tm_u08) / numTm], 0);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.squareWave(SQWAVE_NONE);
  RTC.alarmInterrupt(ALARM_1, true);
  RTC.alarmInterrupt(ALARM_2, true);
}
void setRak() {
  rakSerial.begin(9600);
  delay(100);
  digitalWrite(RAK_RES_PIN, HIGH);
}
void setUsb() {
  if (USBSTA >> VBUS & 1) {    
    usbSerial.begin(9600);    
    while (!usbSerial) {
      wdt_reset();
    }
    usbSerial.flush();
  } 
}   
void doRelay(const uint8_t r, const uint8_t d) {
  if (d == activate) {                  
    digitalWrite(RELAY_PIN[r], HIGH);      
    delay(10);
    digitalWrite(RELAY_PIN[r], LOW);
  } else if (d == deactivate) {           
    digitalWrite(RELAY_PIN[r + 1], HIGH);  
    delay(10);
    digitalWrite(RELAY_PIN[r + 1], LOW);    
  }
}
void report() {
  wdt_reset();
  isReportIftt = false;  
  if (loraJoin && loraSend) {
    loraSend = false;      
    lpp.reset();  
    for (uint8_t ch = 0; ch < numAn; ch++) {
      const uint8_t _unit = conf.an_u08[an_u08_unit + ch * sizeof(conf.an_u08) / numAn];      
      if (_unit == LPP_ANALOG_INPUT) {
        lpp.addAnalogInput(1 + ch, an[ch]);
      } else if (_unit == LPP_LUMINOSITY) {
        lpp.addLuminosity(1 + ch, an[ch]);
      } else if (_unit == LPP_TEMPERATURE) {
        lpp.addTemperature(1 + ch, an[ch]);
      } else if (_unit == LPP_RELATIVE_HUMIDITY) {
        lpp.addRelativeHumidity(1 + ch, an[ch]);
      } else if (_unit == LPP_BAROMETRIC_PRESSURE) {
        lpp.addBarometricPressure(1 + ch, an[ch]);
      } else if (_unit == LPP_VOLTAGE) {
        lpp.addVoltage(1 + ch, an[ch]);
      } else if (_unit == LPP_CURRENT) {
        lpp.addCurrent(1 + ch, an[ch]);
      } else if (_unit == LPP_PERCENTAGE) {
        lpp.addPercentage(1 + ch, an[ch]);
      } else if (_unit == LPP_ALTITUDE) {
        lpp.addAltitude(1 + ch, an[ch]);
      } else if (_unit == LPP_POWER) {
        lpp.addPower(1 + ch, an[ch]);
      } else if (_unit == LPP_DIRECTION) {
        lpp.addDirection(1 + ch, an[ch]);
      } else if (_unit == LPP_DIGITAL_INPUT) {
        lpp.addDigitalInput(1 + ch, an[ch]);
      } else if (_unit == LPP_SWITCH) {
        lpp.addSwitch(1 + ch, an[ch]);
      } else if (_unit == LPP_PRESENCE) {  
        lpp.addPresence(1 + ch, an[ch]);      
      }
    } 
    for (uint8_t ch = 0; ch < numDg; ch++) {
      const uint8_t _unit = conf.dg_u08[dg_u08_unit + ch * sizeof(conf.dg_u08) / numDg];      
      if (_unit == LPP_DIGITAL_INPUT) {
        lpp.addDigitalInput(3 + ch, dg[ch]);
      } else if (_unit == LPP_SWITCH) {
        lpp.addSwitch(3 + ch, dg[ch]);
      } else if (_unit == LPP_PRESENCE) {
        lpp.addPresence(3 + ch, dg[ch]);
      }
    }
    for (uint8_t ch = 0; ch < numMo; ch++) {
      const uint8_t _unit = conf.mo_u08[mo_u08_unit + ch * sizeof(conf.mo_u08) / numMo];      
      if (_unit == LPP_ANALOG_INPUT) {
        lpp.addAnalogInput(5 + ch, mo[ch]);
      } else if (_unit == LPP_LUMINOSITY) {
        lpp.addLuminosity(5 + ch, mo[ch]);
      } else if (_unit == LPP_TEMPERATURE) {
        lpp.addTemperature(5 + ch, mo[ch]);
      } else if (_unit == LPP_RELATIVE_HUMIDITY) {
        lpp.addRelativeHumidity(5 + ch, mo[ch]);
      } else if (_unit == LPP_BAROMETRIC_PRESSURE) {
        lpp.addBarometricPressure(5 + ch, mo[ch]);
      } else if (_unit == LPP_VOLTAGE) {
        lpp.addVoltage(5 + ch, mo[ch]);
      } else if (_unit == LPP_CURRENT) {
        lpp.addCurrent(5 + ch, mo[ch]);
      } else if (_unit == LPP_PERCENTAGE) {
        lpp.addPercentage(5 + ch, mo[ch]);
      } else if (_unit == LPP_ALTITUDE) {
        lpp.addAltitude(5 + ch, mo[ch]);
      } else if (_unit == LPP_POWER) {
        lpp.addPower(5 + ch, mo[ch]);
      } else if (_unit == LPP_DIRECTION) {
        lpp.addDirection(5 + ch, mo[ch]);
      } else if (_unit == LPP_DIGITAL_INPUT) {
        lpp.addDigitalInput(5 + ch, mo[ch]);
      } else if (_unit == LPP_SWITCH) {
        lpp.addSwitch(5 + ch, mo[ch]);
      } else if (_unit == LPP_PRESENCE) {  
        lpp.addPresence(5 + ch, mo[ch]);      
      }
    } 
    rakSerial.print("at+send=lora:" + String(conf.ge_u08[ge_u08_lora_port]) + ':'); 
    //rakSerial.println(lppGetBuffer());
    rakSerial.println((char*)(lpp.getBuffer()));
  } else {
    resetMe();
  }      
}
void delayRandom() {
  randomSeed(analogRead(RANDOM_PIN));
  const unsigned long rnd = random(24) * 5000;   // max 2 minutes
  tmrRandom = millis();
  while (millis() - tmrRandom < rnd) {
    wdt_reset();    
  }
}
void resetMe() {
  wdt_enable(WDTO_15MS);
  while(true); 
}
/*
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
char buf[15];
str.toCharArray(buf, sizeof(buf));
float f = atof(buf);
int32_t i = atol(buf);
// string.toFloat()
// atol() atof()

 */
