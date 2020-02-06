#include <avr/wdt.h>
#include <AltSoftSerial.h>
#include <EEPROM.h>
#include <CayenneLPP.h>
#include <Wire.h>
#include <INA226.h>
#include <DS3232RTC.h>
#include <WebUSB.h>
#include <SensorModbusMaster.h>

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
const uint8_t ge_u16_mod_baud     = 1;  // select 

// uint8_t an_u08[]
const uint8_t an_u08_enable       = 0;  // checkbox
const uint8_t an_u08_unit         = 1;  // select
const uint8_t an_u08_scale        = 2;  // checkbox
const uint8_t an_u08_fall_relay_1 = 3;  // select
const uint8_t an_u08_fall_relay_2 = 4;  // select 
const uint8_t an_u08_fall_report  = 5;  // checkbox
const uint8_t an_u08_rise_relay_1 = 6;  // select
const uint8_t an_u08_rise_relay_2 = 7;  // select 
const uint8_t an_u08_rise_report  = 8;  // checkbox
// uint16_t an_u16[]
const uint8_t an_u16_delay        = 0;  // input
// float an_f32[]
const uint8_t an_f32_in_min       = 0;  // input
const uint8_t an_f32_in_max       = 1;  // input
const uint8_t an_f32_out_min      = 2;  // input
const uint8_t an_f32_out_max      = 3;  // input
const uint8_t an_f32_low_set      = 4;  // input
const uint8_t an_f32_high_set     = 5;  // input

// uint8_t dg_u08[]
const uint8_t dg_u08_enable       = 0;  // checkbox
const uint8_t dg_u08_unit         = 1;  // select
const uint8_t dg_u08_fall_relay_1 = 2;  // select
const uint8_t dg_u08_fall_relay_2 = 3;  // select 
const uint8_t dg_u08_fall_report  = 4;  // checkbox
const uint8_t dg_u08_rise_relay_1 = 5;  // select
const uint8_t dg_u08_rise_relay_2 = 6;  // select 
const uint8_t dg_u08_rise_report  = 7;  // checkbox
// uint16_t dg_u16[]
const uint8_t dg_u16_delay        = 0;  // input

// uint8_t mo_u08[]
const uint8_t mo_u08_enable       = 0;  // checkbox
const uint8_t mo_u08_unit         = 1;  // select
const uint8_t mo_u08_slave        = 2;  // input
const uint8_t mo_u08_function     = 3;  // select
const uint8_t mo_u08_type         = 4;  // select
const uint8_t mo_u08_decimal      = 5;  // input
const uint8_t mo_u08_fall_relay_1 = 6;  // select
const uint8_t mo_u08_fall_relay_2 = 7;  // select 
const uint8_t mo_u08_fall_report  = 8;  // checkbox
const uint8_t mo_u08_rise_relay_1 = 9; // select
const uint8_t mo_u08_rise_relay_2 = 10; // select 
const uint8_t mo_u08_rise_report  = 11; // checkbox
// uint16_t mo_u16[]
const uint8_t mo_u16_register     = 0;  // input
const uint8_t mo_u16_delay        = 1;  // input
// mo_n32[]
const uint8_t mo_n32_low_set      = 0;  // input
const uint8_t mo_n32_high_set     = 1;  // input

// uint8_t tm_u08[]
const uint8_t tm_u08_enable       = 0;  // checkbox
const uint8_t tm_u08_hour         = 1;  // input
const uint8_t tm_u08_minute       = 2;  // input
const uint8_t tm_u08_time-relay_1 = 3;  // select
const uint8_t tm_u08_time-relay_2 = 4;  // select

struct Conf {
  uint8_t   ge_u08[6];
  uint16_t  ge_u16[2];
  uint8_t   an_u08[18];
  uint16_t  an_u16[2];
  float     an_f32[12];
  uint8_t   dg_u08[16];
  uint16_t  dg_u16[2];
  uint8_t   mo_u08[104];
  uint16_t  mo_u16[16];
  uint32_t  mo_u32[16];
  uint32_t  mo_f32[16];
  uint8_t   tm_u08[10];     
};
Conf conf;

struct Va {
  float     an_f32[2];
  uint8_t   dg_u08[2];
  uint32_t  mo_u32[8];
  float     mo_f32[8];  
};
Va va;

const uint8_t u16                 = 0;
const uint8_t i16                 = 1;
const uint8_t u32                 = 2;
const uint8_t i32                 = 3;
const uint8_t f32                 = 4;



void readModbus() {  
  for (uint8_t ch = 0; ch < 8; ch++) {         
    if (conf.mo_u08[mo_u08_enable + ch * sizeof(conf.mo_u08)]) {      
      modbus.begin(mo_u08_slave + ch * sizeof(conf.mo_u08), modbusSerial, RS_DIR_PIN);
      const uint8_t _type = mo_u08_type + ch * sizeof(conf.mo_u08);
      const uint8_t _function = mo_u08_function + ch * sizeof(conf.mo_u08);
      const uint8_t _register = mo_u08_register + ch * sizeof(conf.mo_u08);      
      if (conf.mo_u08[_type] == u16) {        
        va.mo_u32[ch] = modbus.uint16FromRegister(_function, _register), bigEndian);                
      } else if (conf.mo_u08[_type] == i16) {
        va.mo_u32[ch] = modbus.int16FromRegister(_function, _register), bigEndian);                
      } else if if (conf.mo_u08[_type] == u32) {
        va.mo_u32[ch] = modbus.uint32FromRegister(_function, _register), bigEndian);
      } else if if (conf.mo_u08[_type] == i32) {
        va.mo_u32[ch] = modbus.int32FromRegister(_function, _register), bigEndian);
      } else if if (conf.mo_u08[_type] == f32) { 
        va.mo_f32[ch] = modbus.float32FromRegister(_function, _register), bigEndian);     
      }      
    }    
  }  
}

char buf[15];
str.toCharArray(buf, sizeof(buf));
float f = atof(buf);
int32_t i = atol(buf);

// string.toFloat()
// atoul()

///////////////////////////////////////////////////////////////////////////////////
// val[] (12*4 = 48 bytes)
const uint8_t _vl               = 0;          // 4 byte  


const uint8_t _analog           = 0;
const uint8_t _digital          = 1;
const uint8_t _modbus           = 2;
const uint8_t _time             = 3;

const uint8_t _low              = 0;  ///???
const uint8_t _high             = 1;  ///???
const uint8_t _change           = 1;  ///???
const uint8_t _falling          = 2;  ///???
const uint8_t _rising           = 3;  ///???

const uint8_t _activate         = 1;  
const uint8_t _deactivate       = 2;  
const uint8_t _toggle           = 3;  





// type begin 1

////////////////////////////


const uint8_t _amp_min          = 0;
const uint8_t _amp_max          = 1;
const uint8_t _min              = 2;
const uint8_t _max              = 3;
const uint8_t _low_set          = 0;
const uint8_t _low_clear        = 1;
const uint8_t _high_set         = 2;
const uint8_t _high_clear       = 3;
const uint8_t _low              = 0;
const uint8_t _high             = 1;
const uint8_t _unit             = 0;
const uint8_t _pulse_dur        = 0;


const uint8_t _temp             = 1;
const uint8_t _hum              = 2;
const uint8_t _bar              = 3;
const uint8_t _lum              = 4;
const uint8_t _set              = 1;
const uint8_t _res              = 2;
const uint8_t _set_pulse        = 3;
const uint8_t _res_pulse        = 4;

const uint8_t _input            = 0;
const uint8_t _level            = 1;
const uint8_t _channel          = 2;
const uint8_t _relay            = 3;
const uint8_t _uplink           = 5;



struct Alarm {
  uint8_t inp;
  uint8_t lev;
  uint8_t chn;
};

float Amp, Ang[2];
uint8_t hysPrev[2] = {3, 3};
const uint8_t digDly = 10; // ms, max 16ms
bool dsIntPinPrev = HIGH;
unsigned long tmrMillis, tmrMinutes;
String strSerial, strRakSerial;
bool loraJoin = false, loraSend = true;
tmElements_t tm;
uint8_t alarms;


AltSoftSerial rakSerial;
HardwareSerial* modbusSerial = &Serial1;
modbusMaster modbus;
CayenneLPP lpp(51);
INA226 ina;
//WebUSB WebUSBSerial(1 /* https:// */, "leanofis-iot.github.io/lora");
//#define Serial WebUSBSerial

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
  Serial1.begin(conf.ge_u16[ge_u16_mod_baud, SERIAL_8N1);  
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
    readAng(ch);
    calcAngAlr(ch);    
  }  
  if (!DS_INT_PIN) {    
    if (dsIntPinPrev) {
      dsIntPinPrev = LOW;
      calcTmAlr();
    }    
  } else {
    dsIntPinPrev = HIGH;
  }  
  if (alr.inp) {
    doAction();  
  }    
  chkMillis();
  chkRakSerial();
  chkSerial();
  wdt_reset();      
}
void readAng(const uint8_t ch) {
  // wire.end();   
  ina.begin(0x40 + ch);
  Amp = ina.readShuntCurrent();
  if (ina.isAlert()) {}
  Ang[ch] = (Amp - conf.ans_f[_amp_min + ch * 8]) * (conf.ans_f[_max + ch * 8] - conf.ans_f[_min + ch * 8]) / (conf.ans_f[_amp_max + ch * 8] - conf.ans_f[_amp_min + ch * 8]) + conf.ans_f[_min + ch * 8];  
  //(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;  
}
void calcAngAlr(const uint8_t ch) {  
  if (Ang[ch] <= conf.ana_f[_low_set + ch * 8]) {
    if (hysPrev[ch] > 2) {
      alr.inp = _ang;
      alr.lev = _low_set;
      alr.chn = ch;      
    }
    hysPrev[ch] = 1;  
  } else if ((Ang[ch] >= conf.ana_f[_low_clear + ch * 8]) && (Ang[ch] <= conf.ana_f[_high_clear + ch * 8])) {
    if (hysPrev[ch] < 2) {
      alr.inp = _ang;
      alr.lev = _low_clear;
      alr.chn = ch;        
    } else if (hysPrev[ch] > 4) {
      alr.inp = _ang;
      alr.lev = _high_clear;
      alr.chn = ch;      
    }
    hysPrev[ch] = 3; 
  } else if (Ang[ch] >= conf.ana_f[_high_set + ch * 8]) {
    if (hysPrev[ch] < 4) {
      alr.inp = _ang;
      alr.lev = _high_set;
      alr.chn = ch;       
    }
    hysPrev[ch] = 5;    
  }
}
void chkMillis() {
  if (millis() - tmrMillis >= conf.lor_w[_report] * 60000) {
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
        rakSerial.println(conf.lor_b[_dr]);
      } else if (strRakSerial.endsWith("DR" + String(conf.lor_b[_dr]) +" success")) { 
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
      } else if (strSerial.startsWith(F("&lor_b"))) {
        conf.lor_b[strSerial.substring(6,8).toInt()] = strSerial.substring(8).toInt();
      } else if (strSerial.startsWith(F("&lor_w"))) {
        conf.lor_w[strSerial.substring(6,8).toInt()] = strSerial.substring(8).toInt();
      } else if (strSerial.startsWith(F("&rly_w"))) {
        conf.rly_w[strSerial.substring(6,8).toInt()] = strSerial.substring(8).toInt();
      } else if (strSerial.startsWith(F("&anu_b"))) {
        conf.anu_b[strSerial.substring(6,8).toInt()] = strSerial.substring(8).toInt();
      } else if (strSerial.startsWith(F("&ans_f"))) {
        conf.ans_f[strSerial.substring(6,8).toInt()] = strSerial.substring(8).toFloat();
      } else if (strSerial.startsWith(F("&ana_f"))) {
        conf.ana_f[strSerial.substring(6,8).toInt()] = strSerial.substring(8).toFloat();
      } else if (strSerial.startsWith(F("&tma_b"))) {
        conf.tma_b[strSerial.substring(6,8).toInt()] = strSerial.substring(8).toInt();     
      } else if (strSerial.startsWith(F("&alr_b"))) {
        conf.alr_b[strSerial.substring(6,9).toInt()] = strSerial.substring(9).toInt(); 
      } else if (strSerial.startsWith(F("&save"))) {
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
      } else if (strSerial.startsWith(F("&time"))) {
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
  for (uint8_t ii = 0; ii < sizeof(conf.alr_b); ii = ii + 6) {
    if (!conf.alr_b[ii]) {
      alarms = ii;
      break;
    }    
  }    
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
    alr.lev = _high; 
  } else {
    alr.lev = _low;
  }
  alr.inp = _dig;  
  alr.chn = 0;
}
void digAlr1() {
  delayMicroseconds(digDly * 1000);
  if (DIG_PIN[1]) {
    alr.lev = _high; 
  } else {
    alr.lev = _low;
  }
  alr.inp = _dig;  
  alr.chn = 1;
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
  RTC.setAlarm(ALM1_MATCH_HOURS, 0, conf.tma_b[1], conf.tma_b[0], 0);
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, conf.tma_b[3], conf.tma_b[2], 0);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.squareWave(SQWAVE_NONE);
  RTC.alarmInterrupt(ALARM_1, true);
  RTC.alarmInterrupt(ALARM_2, true);
}
void calcTmAlr() {
  if (RTC.alarm(ALARM_1)) {
    alr.inp = _time;
    alr.lev = 0;
    alr.chn = 0;    
  } else if (RTC.alarm(ALARM_2)) {
    alr.inp = _time;
    alr.lev = 0;
    alr.chn = 1;
  }  
}
void setRak() {
  delay(100);
  digitalWrite(RAK_RES_PIN, HIGH);
}
void doAction() {  
  for (uint8_t ii = 0; ii < alarms; ii++) {
    if (conf.alr_b[ii * 6 + _input] == alr.inp) {
      if (conf.alr_b[ii * 6 + _level] == alr.lev) {        
        if (conf.alr_b[ii * 6 + _channel] == alr.chn) {
          for (uint8_t ch = 0; ch < 2; ch++) {            
            actRelay(ch, conf.alr_b[ii * 6 + _relay + ch]);              
          }    
          if (conf.alr_b[ii * 6 + _uplink]) {
            uplink();  
          }
          // break; // ????????????????         
        }
      }        
    }
  }
  alr.inp = 0;
  alr.lev = 0;  // ????????
  alr.chn = 0;  // ????????
}
void actRelay(const uint8_t ch, const uint8_t act) {
  if (act == _set) {                  
    setRelay(ch);
  } else if (act == _res) {           
    resRelay(ch);    
  } else if (act == _set_pulse) {       
    setRelay(ch); 
    delay(conf.rly_w[_pulse_dur + ch]);
    resRelay(ch);
  } else if (act == _res_pulse) {       
    resRelay(ch);
    delay(conf.rly_w[_pulse_dur + ch]);
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
      if (conf.anu_b[ch] == _temp) {
        lpp.addTemperature(1 + ch, Ang[ch]);      
      } else if (conf.anu_b[ch] == _hum) {
        lpp.addRelativeHumidity(1 + ch, Ang[ch]);
      } else if (conf.anu_b[ch] == _bar) {
        lpp.addBarometricPressure(1 + ch, Ang[ch]);
      } else if (conf.anu_b[ch] == _lum) {
        lpp.addLuminosity(1 + ch, Ang[ch]);
      } else {
      lpp.addAnalogInput(1 + ch, Ang[ch]);
      }       
    } 
    for (uint8_t ch = 0; ch < 2; ch++) {      
      lpp.addDigitalInput(3 + ch, digitalRead(DIG_PIN[ch]));      
    } 
    rakSerial.print("at+send=lora:" + String(conf.lor_b[_port]) + ':'); 
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
