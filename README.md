# Tarea 5 Laboratorio: Machine Learning en Sistema Embebido (ESP32)

Este proyecto implementa un sistema de monitoreo y control ON-OFF utilizando un **Perceptrón Simple** entrenado mediante el algoritmo **LMS** sobre un microcontrolador ESP32.

## 👥 Integrantes del Equipo
* **Hector Teruel Grado**
* **Hector Herrera Niño**
* **Ivan Gonzales Salinas**

## 🏗️ Arquitectura del Software
El proyecto está organizado en una arquitectura por capas dentro de la carpeta `src/`:

* **`src/BSP/`**: Board Support Package (Módulos: `ADC.c`, `GPIO.c`, `PRINT.c`, `BSP.h`).
* **`src/ML/`**: Algoritmo del perceptrón simple (Entrenamiento LMS e inferencia).
* **`tarea5_lab_ml.ino`**: Gestión del sistema en tiempo real (RTOS / ciclo principal) y control del estado del botón/LEDs.

## 🚀 Requisitos y Materiales
* Microcontrolador **ESP32**
* Potenciómetros ($N$ entradas)
* 1 Botón
* 2 LEDs (indicadores de estado)
