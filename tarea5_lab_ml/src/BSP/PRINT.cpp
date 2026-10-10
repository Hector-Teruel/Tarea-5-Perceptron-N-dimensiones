/**
 * @file PRINT.cpp
 * @date 10/10/2026
 */

#include "BSP.h"

void print_init(uint32_t baudrate)
{
  Serial.begin(baudrate);
  delay(20);
}

void print_mensaje(const char *msj)
{
  if (msj == NULL) return;

  Serial.println(msj);
}

void print_entero(uint8_t val)
{
  Serial.println(val);
}

void print_float(double val, uint8_t decimales)
{
  Serial.println(val, decimales);
}

bool print_hay_datos()
{
  return (Serial.available() > 0);
}

bool print_leer_comando(char *buffer, size_t max_len)
{
  if (buffer == NULL || max_len == 0) return false;

  if (print_hay_datos()) {
    // Lee hasta encontrar el \n
    String recibido = Serial.readStringUntil('\n');
    recibido.trim();

    if (recibido.length() > 0) {
      recibido.toCharArray(buffer, max_len);
      return true;
    }

    return false;
  }
}