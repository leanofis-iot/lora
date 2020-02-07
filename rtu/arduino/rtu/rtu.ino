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

float             an_f32[2];
uint32_t          mo_u32[8];
float             mo_f32[8]; 

bool              is_iftt;
uint8_t           an_iftt[2];
volatile uint8_t  dg_iftt[2];
uint8_t           mo_iftt[8];
uint8_t           tm_iftt[2]; 

const uint8_t u16                 = 0;
const uint8_t i16                 = 1;
const uint8_t u32                 = 2;
const uint8_t i32                 = 3;
const uint8_t f32                 = 4;

const uint8_t falling             = 1;  
const uint8_t rising              = 2;

const uint8_t _activate           = 1;  
const uint8_t _deactivate         = 2;  
const uint8_t _toggle             = 3; 

unsigned long tmrPoll, tmrReport;
String strSerial, strRakSerial;
bool loraJoin = false, loraSend = true;
tmElements_t tm;

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
  setAnalog();
  setDigital();
  setModbus();
  setTm();
  //delayRandom();    
  setRak(); 
  tmrMillis = millis();
}
void loop() {
  if (isPollInterval()) {
    getAnalog();
    getModbus();
  }  
  getDigital();
  getTm();
  if (is_iftt) {
    doIftt();  
  }
  if (isReportInterval()) {
    report();    
  }  
  doRakSerial();
  doSerial();
  wdt_reset();
}
void getAnalog() {
  // wire.end(); 
  for (uint8_t ch = 0; ch < 2; ch++) {    
    while (INA_ALR_PIN[ch]);
    ina.begin(0x40 + ch);
    an_f32[ch] = ina.readShuntCurrent();
    if (ina.isAlert());
    const uint8_t _enable = an_u08_enable + ch * sizeof(conf.an_u08];
    if (conf.an_u08[_enable]) {
      const uint8_t _in_min = an_f32_in_min + ch * sizeof(conf.an_f32];
      const uint8_t _in_max = an_f32_in_max + ch * sizeof(conf.an_f32];
      const uint8_t _out_min = an_f32_out_min + ch * sizeof(conf.an_f32];
      const uint8_t _out_max = an_f32_out_max + ch * sizeof(conf.an_f32];
      an_f32[ch] = (an_f32[ch] - conf[_in_min]) * (conf[_out_max] - conf[_out_min)] / (conf[_in_max] - conf[_in_min]) + conf[_out_min];      
    }              
  }    
}
void getDigital() {
  for (uint8_t ch = 0; ch < 2; ch++) {
    const uint8_t _enable = dg_u08_enable + ch * sizeof(conf.dg_u08];
    if (conf.dg_u08[_enable]) {
      if (dg_iftt[ch]) {
        const uint8_t _delay = dg_u16_delay + ch * sizeof(conf.dg_u16];
        delay(dg_u16[_delay]);
        dg_iftt[ch] = digitalRead(DIG_PIN[ch]) ? 1 : 2;        
      }      
    }
  }
}
void getModbus() {  
  for (uint8_t ch = 0; ch < 8; ch++) {
    const uint8_t _enable = mo_u08_enable + ch * sizeof(conf.mo_u08);        
    if (conf.mo_u08[_enable]) { 
      const uint8_t _slave = mo_u08_slave + ch * sizeof(conf.mo_u08);     
      modbus.begin(conf.mo_u08[_slave], modbusSerial, RS_DIR_PIN);
      const uint8_t _type = mo_u08_type + ch * sizeof(conf.mo_u08);
      const uint8_t _function = mo_u08_function + ch * sizeof(conf.mo_u08);
      const uint8_t _register = mo_u16_register + ch * sizeof(conf.mo_u16);      
      if (conf.mo_u08[_type] == u16) {        
        mo_u32[ch] = modbus.uint16FromRegister(conf.mo_u08[_function], conf.mo_u16[_register], bigEndian);                
      } else if (conf.mo_u08[_type] == i16) {
        mo_u32[ch] = modbus.int16FromRegister(conf.mo_u08[_function], conf.mo_u16[_register]), bigEndian);                
      } else if if (conf.mo_u08[_type] == u32) {
        mo_u32[ch] = modbus.uint32FromRegister(conf.mo_u08[_function], conf.mo_u16[_register], bigEndian);
      } else if if (conf.mo_u08[_type] == i32) {
        mo_u32[ch] = modbus.int32FromRegister(conf.mo_u08[_function], conf.mo_u16[_register], bigEndian);
      } else if if (conf.mo_u08[_type] == f32) { 
        mo_f32[ch] = modbus.float32FromRegister(conf.mo_u08[_function], conf.mo_u16[_register], bigEndian);     
      }      
    }    
  }  
}
void getTm() {
  for (uint8_t ch = 0; ch < 2; ch++) {
    const uint8_t _enable = tm_u08_enable + ch * sizeof(conf.tm_u08];
    if (conf.tm_u08[_enable]) {
      if (!DS_INT_PIN) {
        if (RTC.alarm(ALARM_1)) {
          tm_iftt[0] = 1;
        } else if (RTC.alarm(ALARM_2)) {
          tm_iftt[1] = 1;
        } 
      } 
    }      
  }
}
bool isPollInterval() {
  const uint8_t _poll = ge_u08_poll;
  if (millis() - tmrPoll >= conf.ge_u08[_poll] * 1000) {
    tmrPoll = millis();
    return true;
  }
}
bool isReportInterval() {
  const uint8_t _report = ge_u16_report;
  if (millis() - tmrReport >= conf.ge_u16[_report] * 60000) {
    tmrReport = millis();
    return true;
  }
}
void doRakSerial() {
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
void doSerial() {
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
void setAnalog() {
  for (uint8_t ch = 0; ch < 2; ch++) { 
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
  dg_iftt[0] = 1;  
}
void digChange1() {
  dg_iftt[1] = 1;
}
void setModbus() {
  Serial1.begin(conf.ge_u16[ge_u16_mod_baud, SERIAL_8N1);
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
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, conf.tm_u08[tm_u08_minute + sizeof(conf.tm_u08)], conf.tm_u08[tm_u08_hour + sizeof(conf.tm_u08)], 0);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.squareWave(SQWAVE_NONE);
  RTC.alarmInterrupt(ALARM_1, true);
  RTC.alarmInterrupt(ALARM_2, true);
}
void setRak() {
  delay(100);
  digitalWrite(RAK_RES_PIN, HIGH);
}
void doIftt() {  
  
}
void doRelay(const uint8_t ch, const uint8_t do) {
  if (do == _set) {                  
    setRelay(ch);
  } else if (do == _res) {           
    resRelay(ch);    
  } else if (do == _set_pulse) {       
    setRelay(ch); 
    delay(conf.rly_w[_pulse_dur + ch]);
    resRelay(ch);
  } else if (do == _res_pulse) {       
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
void report() {
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
/*
 * char buf[15];
str.toCharArray(buf, sizeof(buf));
float f = atof(buf);
int32_t i = atol(buf);
// string.toFloat()
// atol() atof()
*/
 */
