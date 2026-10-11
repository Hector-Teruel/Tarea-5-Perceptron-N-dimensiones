#include "ADC.h"

typedef struct{
  Gpio_array bsp_pots;
  int bsp_threshold;
} bsp_pots_config;

typedef struct{
  Readings_array bsp_analog;
  Readings_array bsp_encoded;
} bsp_pots_readings;


class bsp_board_pots {
  private: 
    bsp_pots_config bsp_setup;
    //int bsp_ON_pots;

  public:
    bsp_board_pots(Gpio_array pines, int threshold){
      setup_analog_read();
      bsp_setup.bsp_pots = pines;
      bsp_setup.bsp_threshold = threshold;
    }

    bsp_pots_readings bsp_read_pots(){
      bsp_pots_readings pot_readings;
      Readings_array read_all_pots = read_analog_pines(bsp_setup.bsp_pots);

      for (int i=0; i<read_all_pots.size(); i++){
        if (read_all_pots[i] != 0){
          if (read_all_pots[i] < bsp_setup.bsp_threshold){
            pot_readings.bsp_encoded.push_back(0);
          }
          else{
            pot_readings.bsp_encoded.push_back(1);
          }
          pot_readings.bsp_analog.push_back(read_all_pots[i]);
        }

      }
      //bsp_ON_pots = hard_binary_readings.size();
      return pot_readings;
    }

    /*int get_pots_ON() {
      return bsp_ON_pots;
    }*/
};

Gpio_array bsp_gpio_pots = {34, 35, 32, 33, 39};
int bsp_user_threshold = 1548; // max_read = 4096, min_read = 1000, range = 3096, middle = 1548

bsp_board_pots bsp_pots(bsp_gpio_pots, bsp_user_threshold);

void setup() {
  Serial.begin(115200);
}

void loop() {
  bsp_pots_readings readings = bsp_pots.bsp_read_pots();
  //int pots_funcionando = bsp_pots.get_pots_ON();
  int pots_funcionando = readings.bsp_encoded.size();

  Serial.println(pots_funcionando);

  for (int i = 0; i < readings.bsp_encoded.size(); i++) {
    Serial.print(readings.bsp_encoded[i]);
    Serial.print(",");
    Serial.println(readings.bsp_analog[i]);
  }
    
  Serial.println("---");
  delay(500);
}
