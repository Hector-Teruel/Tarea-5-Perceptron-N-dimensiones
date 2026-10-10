/**
 * @file ADC.cpp
 * @date 07/10/2026
 */

#include "BSP.h"

// Lista para mapeo de los pines de los pots
static const pots_e LISTA_POTS[] = {POT1, POT2, POT3, POT4, POT5};

static uint8_t n_entradas_act = 0;
static pots_e *pots_act = NULL;

void adc_init()
{
  // Configurar resolucion
  analogReadResolution(12);

  // Configurar atenuacion
  analogSetAttenuation(ADC_11db);

  // Configurar como entradas
  for (uint8_t i = 0; i < 5; i++) {
    pinMode(LISTA_POTS[i], INPUT);
  }
}

uint8_t adc_detectar_entradas()
{
  pots_e temp[5];

  for (uint8_t i = 0; i <  5; i++) {
    uint8_t suma = 0;

    for (uint8_t j = 0; j < 10; j++) {
      suma += analogRead(LISTA_POTS[i]);
      delay(2);
    }

    uint16_t prom = (uint16_t)(suma / 10);

    if (prom > 80) {
      n_entradas_act++;
      temp[i] = LISTA_POTS[i];
    }
  }

  if (n_entradas_act < 2) n_entradas_act = 2;
  
  pots_act = (pots_e *)malloc((size_t)n_entradas_act * sizeof(uint8_t));
  if (pots_act == NULL) {
    free(pots_act);
    return 0;
  }

  for (uint8_t i = 0; i < n_entradas_act; i++) {
    pots_act[i] = temp[i];
  }
}

uint8_t adc_leerBin_act(uint8_t ind, uint16_t umbral)
{
  if (ind > n_entradas_act) return 0;

  pots_e pot = pots_act[ind];
  return (analogRead(pot) >= umbral) ? 1 : 0;
}