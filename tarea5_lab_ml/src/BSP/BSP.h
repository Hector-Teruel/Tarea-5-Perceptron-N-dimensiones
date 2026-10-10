/**
 * @file BSP.h
 * @date 07/10/2026
 */

#ifndef BSP_H
#define BSP_H

#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

#endif