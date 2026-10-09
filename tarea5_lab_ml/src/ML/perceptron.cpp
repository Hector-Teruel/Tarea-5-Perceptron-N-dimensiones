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
 p->d_bin = (uint8_t *)malloc((size_t)p->patrones * sizeof(uint8_t));
 p->w = (double *)calloc((size_t)p->entradas, sizeof(double));

 if (p->X == NULL || p->d == NULL || p->d_bin == NULL || p->w == NULL) {
   perceptron_free(p);
   return false;
 }

 return true;
}

void perceptron_free(perceptron_t *p)
{
  if (p == NULL) return;

  if (p->X != NULL)     free(p->X);     p->X = NULL;
  if (p->d != NULL)     free(p->d);     p->d = NULL;
  if (p->d_bin != NULL) free(p->d_bin); p->d_bin = NULL;
  if (p->w != NULL)     free(p->w);     p->w = NULL;
}