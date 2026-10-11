/*
 * 
 */

#include "ADC.h"

void setup() {
    Serial.begin(115200);
    setup_analog_read();
}

void loop() {
  Gpio_array pines = {34, 35, 32, 33, 39};
  Readings_array readings = read_analog_pines(pines);

  for (int i = 0; i < readings.size(); i++) {
    Serial.println(readings[i]);
  }
    
  Serial.println("---");
  delay(500);
}
