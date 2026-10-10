/**
 * @file GPIO.cpp
 * @date 10/10/2026
 *
 * @brief Implementacion de las funciones del 
 * componente GPIO
 */

#include "BSP.h"

void gpio_init()
{
  // Configuracion de GPIO de salida
  pinMode(LED_STATE, OUTPUT);   digitalWrite(LED_STATE, LOW);
  pinMode(LED_SALIDA, OUTPUT);  digitalWrite(LED_SALIDA, LOW);

  // Configuracion de GPIO de entrada
  pinMode(BOTON, INPUT_PULLUP);
}

bool leer_boton()
{
  // Lectura boton pullup
  uint8_t lec = digitalRead(BOTON);
  
  if (lec == 0) return true;
  else if (lec == 1) return false;
  else return false;
}

void encender_led(leds_e led)
{
  switch (led) {
    case LED_STATE:   digitalWrite(LED_STATE, HIGH);  break;
    case LED_SALIDA:  digitalWrite(LED_SALIDA, HIGH); break;
    default: return;
  }
}

void apagar_led(leds_e led)
{
  switch (led) {
    case LED_STATE:   digitalWrite(LED_STATE, LOW);  break;
    case LED_SALIDA:  digitalWrite(LED_SALIDA, LOW); break;
    default: return;
  }
}

void toggle_led(leds_e led)
{
  switch (led) {
    case LED_STATE:   digitalWrite(LED_STATE, !digitalRead(LED_STATE));   break;
    case LED_SALIDA:  digitalWrite(LED_SALIDA, !digitalRead(LED_SALIDA)); break;
    default: return;
  }
}