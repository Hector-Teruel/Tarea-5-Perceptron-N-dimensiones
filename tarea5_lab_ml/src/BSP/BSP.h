/**
 * @file BSP.h
 * @date 07/10/2026
 *
 * @brief Archivo de cabecera que contiene las librerias, enumeraciones
 * y prototipos de funciones necesarias para los componentes del sistema:
 * - ADC
 * - GPIO
 * - PRINT
 */

#ifndef BSP_H
#define BSP_H

#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define BOTON 26
#define UMBRAL 1548 //2048

// =================================================================
//                  DEFINICION DE ENUMERACIONES
// =================================================================

typedef enum {
  OFF = 0,
  RUN,
} modo_sis_e;

typedef enum {
  POT1 = 36,
  POT2 = 39,
  POT3 = 34,
  POT4 = 35,
  POT5 = 32
} pots_e;

typedef enum {
  LED_STATE = 2,
  LED_SALIDA = 25
} leds_e;

// =================================================================
//                  PROTOTIPOS DE FUNCIONES GPIO
// =================================================================

/**
 * @brief Funcion de inicializacion de los GPIOs del sistema
 */
void gpio_init();

/**
 * @brief Funcion para lectura del boton del sistema
 *
 * @return Devuelve true: si fue presionado, false: si no fue presionado
 */
bool leer_boton();

/**
 * @brief Funcion para encender un led
 *
 * @param led LED que se quiere encender
 */
void encender_led(leds_e led);

/**
 * @brief Funcion para apagar un led
 *
 * @param led LED que se quiere apagar
 */
void apagar_led(leds_e led);

/**
 * @brief Funcion para alternar el estado de un led
 *
 * @param led LED que se quiere alternar su estado
 */
void toggle_led(leds_e led);

// =================================================================
//                  PROTOTIPOS DE FUNCIONES ADC
// =================================================================

/**
 * @brief Funcion que inicializa los canales analogicos del ESP32
 */
void adc_init();

/**
 * @brief Funcion para dectectar la cantidad de pots conectados
 *
 * @return uint8_t Numero de entradas detectadas
 */
uint8_t adc_detectar_entradas();

/**
 * @brief Lee el valor binarizado del j-ésimo potenciómetro detectado como activo
 * @param indice_activo Índice de entrada (0 a N-1)
 * @param umbral Valor límite (ej. 2048)
 */
uint8_t adc_leerBin_act(uint8_t ind, uint16_t umbral);

/**
 * @brief Libera la memoria dinamica reservada y reinicia el valor
 * de entradas activas a cero
 */
void adc_free();

// =================================================================
//               PROTOTIPOS DE FUNCIONES PRINT / UART
// =================================================================

/**
 * @brief Inicializa la comunicacion por puerto Serial con la HMI
 * @param baudrate Velocidad en baudios (ej. 115200)
 */
void print_init(uint32_t baudrate);

/**
 * @brief Envia una cadena de texto a la HMI
 * @param msj Mensaje a transmitir con salto al final 
 */
void print_mensaje(const char *msj);

/**
 * @brief Envia una cadena de texto a la HMI
 * @param msj Mensaje a transmitir sin salto al final 
 */
void print_mensaje_sinln(const char *msj);

/**
 * @brief Envia un numero entero a la HMI
 * @param val Entero a transmitir
 */
void print_entero(int val);

/**
 * @brief Envia un numero con punto flotante a la HMI con precision fija
 * @param val Decimal a transmitir
 * @param decimales Cantidad de digitos decimales
 */
void print_float(double val, uint8_t decimales);

/**
 * @brief Verifica si la HMI ha enviado un comando por el puerto serie
 * @return true: si hay datos en el buffer de entrada, false: en caso contrario
 */
bool print_hay_datos();

/**
 * @brief Lee una linea completa de comando enviada desde la HMI
 * @param buffer Arreglo donde se guarda la cadena recibida
 * @param max_len Tamaño maximo del buffer
 * @return true: si se lee una cadena valida, false: si no es valida
 */
bool print_leer_comando(char *buffer, size_t max_len);

#endif