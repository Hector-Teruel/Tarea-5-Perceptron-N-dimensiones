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

/**
 * @struct perceptron_t
 * @brief Estructura principal del perceptron
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
  uint16_t max_epocas;          // Maximo de epocas

  // Metricas del entrenamiento
  double mse_final;             // Error cuadratico medio final
  uint16_t epocas_realizadas;   // Epocas que tomo entrenar
  bool convergio;               // Bandera si convergio
  bool divergio;                // Bandera si divergio
  uint8_t aciertos;             // Clasificaciones correctas finales
} perceptron_t;


bool perceptron_init(perceptron_t *p, uint8_t entradas);

void perceptron_free(perceptron_t *p);

double producto_punto(const perceptron_t *p);

void generar_patrones(perceptron_t *p);

uint8_t contar_aciertos(const double *X, const uint8_t *d_bin, const double *w, double b, int entradas, uint8_t patrones);

void entrenarLMS(perceptron_t *p, uint8_t clase_min, double factor_min);
#endif