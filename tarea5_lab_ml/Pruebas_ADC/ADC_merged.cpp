/*
 *  ADC all together
 */

#include <Arduino.h>
#include <vector>

#define Gpio_array std::vector<int>
#define Readings_array std::vector<int>

const int numEntradas = 5;

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