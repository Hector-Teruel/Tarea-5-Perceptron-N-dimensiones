#include "ADC.h"

typedef struct{
  Gpio_array bsp_pots;
  int bsp_threshold;
} bsp_pots_config;


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

    Readings_array bsp_read_pots(){
      Readings_array hard_binary_readings;
      Readings_array readings = read_analog_pines(bsp_setup.bsp_pots);

      for (int i=0; i<readings.size(); i++){
        if (readings[i] != 0){
          if (readings[i] < bsp_setup.bsp_threshold){
            hard_binary_readings.push_back(0);
          }
          else{
            hard_binary_readings.push_back(1);
          }
        }

      }
      //bsp_ON_pots = hard_binary_readings.size();
      return hard_binary_readings;
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
  Readings_array readings = bsp_pots.bsp_read_pots();
  //int pots_funcionando = bsp_pots.get_pots_ON();
  int pots_funcionando = readings.size();

  Serial.println(pots_funcionando);

  for (int i = 0; i < readings.size(); i++) {
    Serial.println(readings[i]);
  }
    
  Serial.println("---");
  delay(500);
}
