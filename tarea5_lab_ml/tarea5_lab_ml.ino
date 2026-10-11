/**
 * @file main.ino
 * @author Hector Teruel Grado
 * @author Hector Herrera Niño
 * @author Ivan Gonzalez Salinas
 * @date 10/10/2026
 * @brief Laboratorio 2 - ML en Sistema Embebido
 */

#include "src/BSP/BSP.h"
#include "src/ML/perceptron.h"

// =================================================================
//                  PROTOTIPOS DE FUNCIONES 
// =================================================================

void tareaBoton(void *pvParameters);

void tareaEntrenamiento(void *pvParameters);

void tareaInferencia(void *pvParameters);

void tareaHMI(void *pvParameters);

// =================================================================
//            VARIABLES GLOBALES Y HANDLERS RTOS
// =================================================================

// Variable del modo del sistema
volatile modo_sis_e modo_sis = OFF;

// Estructura para almacenar el modelo entrenado
modelo_t modelo_sistema;

// Variables para mostrar metricas por la HMI
uint8_t hmi_aciertos = 0;
double hmi_mse = 0.0;
uint16_t hmi_epocas = 0;
uint8_t num_entradas_detectadas = 2;

bool flag_nuevo_entrenamiento = false;

void setup() {
  // Inicializar componentes
  gpio_init();
  adc_init();
  print_init(115200);

  // Detectar la cantidad de entradas conectadas
  num_entradas_detectadas = adc_detectar_entradas();

  // Inicializar el modelo ligero para inferencia
  modelo_init(&modelo_sistema, num_entradas_detectadas);
}

void loop() {
  // RTOS
  vTaskDelay(portMAX_DELAY);
}
