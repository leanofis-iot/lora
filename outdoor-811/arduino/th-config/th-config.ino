#include <EEPROM.h>

struct Conf {
  const uint16_t read_p = 1;
  const uint16_t send_p = 30;
  const float alr_lo_t = 5;
  const float alr_hi_t = 50;
  const float alr_hys_t = 5;
  const float alr_lo_h = 15;
  const float alr_hi_h = 70;
  const float alr_hys_h = 5;
  const uint8_t dr = 2;      
};

Conf conf;

void setup() {
  EEPROM.put(0, conf);
}

void loop() {
}
