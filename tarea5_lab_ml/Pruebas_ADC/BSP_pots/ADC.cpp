#include "ADC.h"

void setup_analog_read(){
  analogReadResolution(12);
}

Readings_array read_analog_pines(const Gpio_array& pines){ //using & alias to avoid copying the vector
  Readings_array analog_readings;

  for (int i = 0; i < pines.size(); i++) {
        int lectura = analogRead(pines[i]);
        analog_readings.push_back(lectura);
    }

    return analog_readings;
}