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
//            VARIABLES GLOBALES Y HANDLERS RTOS
// =================================================================

// Variable del modo del sistema
volatile modo_sis_e modo_sis = OFF;

// Estructura para almacenar el modelo entrenado
modelo_t modelo_sistema;

// Configuracion de entrenamiento desde HMI
op_salida_e seleccion = AND;  // op = AND por default
char secuencia[33] = {0}; // 2^5 = 32B + 1 por '\0'

// Variables para mostrar metricas por la HMI
uint8_t hmi_aciertos = 0;
double hmi_mse = 0.0;
uint16_t hmi_epocas = 0;
uint8_t num_entradas_detectadas = 2;
bool bandera_entrenado = false;

SemaphoreHandle_t semEntrenamiento = NULL;
SemaphoreHandle_t mtxModelo = NULL;

// =================================================================
//                  PROTOTIPOS DE FUNCIONES 
// =================================================================

void tareaBoton(void *pvParameters);
void tareaEntrenamiento(void *pvParameters);
void tareaInferencia(void *pvParameters);
void tareaHMI(void *pvParameters);

// =================================================================
//                             SETUP
// =================================================================

void setup() {
  // Inicializar componentes
  gpio_init();
  adc_init();
  print_init(115200);

  // Detectar la cantidad de entradas conectadas
  num_entradas_detectadas = adc_detectar_entradas();

  // Inicializar el modelo ligero para inferencia
  modelo_init(&modelo_sistema, num_entradas_detectadas);

  // Crear semaforo y mutex
  semEntrenamiento = xSemaphoreCreateBinary();
  mtxModelo = xSemaphoreCreateMutex();

  // Crear las tareas
  xTaskCreatePinnedToCore(tareaBoton, "TareaBoton", 2048, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(tareaEntrenamiento, "TareaEntrenamiento", 8192, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(tareaInferencia, "TareaInferencia", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(tareaHMI, "TareaHMI", 3072, NULL, 1, NULL, 0);

  print_mensaje("[Sistema] Sistema iniciado en modo OFF. Esperando...");
}

void loop() {
  // RTOS
  vTaskDelay(portMAX_DELAY);
}

// =================================================================
//                     IMPLEMENTACION DE TAREAS
// =================================================================

void tareaBoton(void *pvParameters)
{
  bool state_old = false;

  for (;;) {
    bool state = leer_boton();

    // Detectar flanco de bajada solamente
    if (state && !state_old) {
      vTaskDelay(pdMS_TO_TICKS(50));  // debounce simple

      if (state) {
        if (modo_sis == OFF) {
          modo_sis = RUN;
          encender_led(LED_STATE);

          print_mensaje("[Sistema] Modo RUN activado. Seleccione la funcion y presione 'Enviar'.");
        }

        else {
          modo_sis = OFF;
          apagar_led(LED_STATE);
          apagar_led(LED_SALIDA);

          print_mensaje("[Sistema] Modo OFF activado. Sistema en espera.");
        }
      }
    }

    state_old = state;
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void tareaEntrenamiento(void *pvParameters)
{
  perceptron_t perceptron;

  for (;;) {
    // Esperar señal de arranque
    if (xSemaphoreTake(semEntrenamiento, portMAX_DELAY) == pdTRUE) {
      if (modo_sis == RUN) {
        print_mensaje("[ML] Iniciando memoria dinamica y entrenamiento.");

        // Memoria dinamica
        if (perceptron_init(&perceptron, num_entradas_detectadas)) {
          generar_patrones(&perceptron);

          // Cargar salidas
          cargar_salidas(&perceptron, seleccion, secuencia);

          // Entrenar
          entrenarLMS(&perceptron, &modelo_sistema);

          // Guardar metricas
          if (xSemaphoreTake(mtxModelo, portMAX_DELAY) == pdTRUE) {
            hmi_aciertos = perceptron.aciertos;
            hmi_mse = perceptron.mse_final;
            hmi_epocas = perceptron.epocas_realizadas;
            bandera_entrenado = true;

            xSemaphoreGive(mtxModelo);
          }

          // Liberar memoria tras entrenamiento
          perceptron_free(&perceptron);
          print_mensaje("[ML] Entrenamiento completado.");
        }

        else {
          print_mensaje("[Error] Fallo en asignar memoria al iniciar perceptron_t.");
        }
      }
    }
  }
}

void tareaInferencia(void *pvParameters)
{
  char hmi_pots[16];
  
  for (;;) {
    if (modo_sis == RUN) {
      if (xSemaphoreTake(mtxModelo, pdMS_TO_TICKS(10)) == pdTRUE) {
        if (modelo_sistema.entrenado) {
          double x_in[num_entradas_detectadas];

          // Construir encabezado para HMI
          snprintf(hmi_pots, sizeof(hmi_pots), "POTS:");
          
          for (uint8_t j = 0; j < num_entradas_detectadas; j++) {
            uint8_t val_bin = adc_leerBin_act(j, UMBRAL);
            x_in[j] = (double)val_bin;

            // Concatenar trama
            size_t len = strlen(hmi_pots);
            if (len < sizeof(hmi_pots) - 1) {
              hmi_pots[len] = val_bin ? '1' : '0';
              hmi_pots[len + 1] = '\0';
             }
          }

          // Inferencia
          uint8_t inf = modelo_inferir(&modelo_sistema, x_in);

          if (inf == 1) encender_led(LED_SALIDA);
          else apagar_led(LED_SALIDA);

          print_mensaje(hmi_pots);
        }

        xSemaphoreGive(mtxModelo);
      }

      else {
        apagar_led(LED_SALIDA);
      }

      vTaskDelay(pdMS_TO_TICKS(70));
    }
  }
}

void tareaHMI(void *pvParameters)
{
  char buffer[64];

  for (;;) {
    if (print_leer_comando(buffer, sizeof(buffer))) {
      // Ver opcion seleccionada
      if (strncmp(buffer, "CMD:AND", 7) == 0) {
        seleccion = AND;
        secuencia[0] = '\0';

        print_mensaje("[HMI] Configuracion cargada: Funcion AND.");
      }

      else if (strncmp(buffer, "CMD:OR", 6) == 0) {
        seleccion = OR;
        secuencia[0] = '\0';

        print_mensaje("[HMI] Configuracion cargada: Funcion OR.");
      }

      else if (strncmp(buffer, "CMD:OTRO", 9) == 0) {
        seleccion = OTRO;
        strncpy(secuencia, &buffer[9], sizeof(secuencia) - 1);
        secuencia[sizeof(secuencia) - 1] = '\0'; // al final de la secuencia poner '\0'

        print_mensaje("[HMI] Configuracion cargada: Funcion personalizada.");
      }

      if (modo_sis == RUN) {
        xSemaphoreGive(semEntrenamiento);
      }

      else {
        print_mensaje("[HMI] El sistema esta en OFF. Active con el boton.");
      }
    }

    if (bandera_entrenado) {
      if (xSemaphoreTake(mtxModelo, portMAX_DELAY) == pdTRUE) {
        print_mensaje("===== METRICAS DE ENTRENAMIENTO =====");
        print_mensaje_sinln("Entradas activas: "); print_entero(num_entradas_detectadas);
        print_mensaje_sinln("Aciertos: "); print_entero(hmi_aciertos);
        print_mensaje_sinln("Epocas: "); print_entero(hmi_epocas);
        print_mensaje_sinln("MSE Final: "); print_float(hmi_mse, 6);
        print_mensaje("=====================================");

        bandera_entrenado = false;
        xSemaphoreGive(mtxModelo);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}