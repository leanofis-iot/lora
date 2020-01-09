#include <EEPROM.h>

struct Conf {
  uint8_t bytes[];   // an0_en, an1_en, dig0_type, dig1_type, dr 
  uint16_t words[1];  // send_p
  float floats[14];    // alr0_min, alr0_max, alr1_min, alr1_max, hys0, hys1, in0_min, in0_max, in1_min, in1_max, val0_min, val0_max, val1_min, val1_max
};

Conf conf;

void setup() {
  EEPROM.put(0, conf);
}

void loop() {
}
