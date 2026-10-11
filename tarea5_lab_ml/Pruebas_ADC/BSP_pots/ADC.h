#ifndef ADC_H
#define ADC_H

#include <Arduino.h>
#include <vector>

#define Gpio_array std::vector<int>
#define Readings_array std::vector<int>

void setup_analog_read();
Readings_array read_analog_pines(const Gpio_array& pines);

#endif