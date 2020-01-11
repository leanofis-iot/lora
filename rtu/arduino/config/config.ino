#include <EEPROM.h>

struct Conf {
  uint8_t bytes[38] = {0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,2,1};   
  uint16_t words[3] = {30,1000,1000};  
  float floats[16]  = {0.004,0.004,0.020,0.020,0,0,100,100,10,10,9,9,50,50,49,49};
};

Conf conf;

void setup() {
  EEPROM.put(0, conf);
}

void loop() {
}
