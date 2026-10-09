/**
 * @file perceptron.cpp
 * @date 08/10/2026
 */

 #include "perceptron.h"

 bool perceptron_init(perceptron_t *p, uint8_t entradas)
 {
  if (p == NULL || entradas < 2) return false;

  p->entradas = entradas;
  p->patrones = 1U << entradas; // 2^entradas
  p->eta = 0.1;
  p->max_epocas = MAX_EPOCAS;
  p->b = 0.0;

  // Asignacion de memoria dinamica
  p->X = (double *)malloc((size_t)p->patrones * p->entradas * sizeof(double));
  p->d = (double *)malloc((size_t)p->patrones * sizeof(double));
  //p->d_bin = 
 }