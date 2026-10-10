/**
 * @file perceptron.h
 * @date 08/10/2026
 */

#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define MAX_ENTRADAS 5
#define MAX_EPOCAS 5000
#define TOLERANCIA 1e-12
#define PACIENCIA 20

// =================================================================
// DEFINICION DE ESTRUCTURAS
// =================================================================
/**
 * @struct perceptron_t
 * @brief Estructura principal del perceptron que contiene todo lo 
 * que necesita para el entrenamiento
 */
typedef struct {
  uint8_t entradas;             // Cantidad de entradas N (2 a 5)
  uint8_t patrones;             // Cantidad de combinaciones (2^N)

  double *X;                    // Entradas
  double *d;                    // Salida deseada continua
  uint8_t *d_bin;               // Salida deseada binaria (0 o 1)

  double *w;                    // Vector de pesos
  double b;                     // Bias

  double eta;                   // Learning rate

  // Metricas del entrenamiento
  double mse_final;             // Error cuadratico medio final
  uint16_t epocas_realizadas;   // Epocas que tomo entrenar
  bool convergio;               // Bandera si convergio
  bool divergio;                // Bandera si divergio
  uint8_t aciertos;             // Clasificaciones correctas finales
} perceptron_t;

/**
 * @struct modelo_t
 * @brief Estructura del modelo ya entrenado, unicamente contiene
 * lo que necesita para la realizar la inferencia
 */
typedef struct {
  uint8_t entradas;
  double *w;
  double b;
  bool entrenado;
} modelo_t;

/**
 * @enum opcion_salida_e
 * @brief Enumeracion con las opciones del tipo de salida que se quiere
 */
typedef enum {
  AND = 0,
  OR,
  OTRO
} op_salida_e;

// =================================================================
// PROTOTIPOS DE FUNCIONES
// =================================================================

// Inicializacion y liberacion de memoria
bool perceptron_init(perceptron_t *p, uint8_t entradas);
void perceptron_free(perceptron_t *p);

bool modelo_init(modelo_t *m, uint8_t entradas);
void modelo_free(modelo_t *m);

// Operaciones matematicas
double producto_punto(const double *w, const double *x, uint8_t entradas);

// Generacion de tabla de verdad
void generar_patrones(perceptron_t *p);

// Carga de la salida deseada
void cargar_salidas(perceptron_t *p, op_salida_e opcion, const char *secuencia);

// Entrenamiento LMS
uint8_t contar_aciertos(const perceptron_t *p, const double *w_actual, double b_actual);
void entrenarLMS(perceptron_t *p, modelo_t *m);

// Inferencia para la tarea en modo RUN
uint8_t modelo_inferir(const modelo_t *m, const double *x_in);
#endif