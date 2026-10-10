/**
 * @file perceptron.cpp
 * @date 08/10/2026
 */

#include "perceptron.h"

bool perceptron_init(perceptron_t *p, uint8_t entradas)
{
  if (p == NULL || entradas < 2 || entradas > MAX_ENTRADAS) return false;
  
  p->entradas = entradas;
  p->patrones = 1U << entradas; // 2^entradas
  p->eta = 0.1;
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

bool modelo_init(modelo_t *m, uint8_t entradas)
{
  if (m == NULL || entradas < 2 || entradas > MAX_ENTRADAS) return false;

  m->entradas = entradas;

  m->w = (double *)calloc((size_t)m->entradas, sizeof(double));
  m->b = 0.0;
  m->entrenado = false;
  
  if (m->w == NULL) {
    modelo_free(m);
    return false;
  }

  return true;
}

void modelo_free(modelo_t *m)
{
  if (m == NULL) return;

  if (m->w != NULL) free(m->w); m->w = NULL;
}

double producto_punto(const double *w, const double *x, uint8_t entradas)
{
  double suma = 0.0;

  for (int j = 0; j < entradas; j++) {
    suma += w[j] * x[j];
  }

  return suma;
}

void generar_patrones(perceptron_t *p)
{
  if (p == NULL) return;
  
  for (uint8_t i = 0; i < p->patrones; i++) {
    for (uint8_t j = 0; j < p->entradas; j++) {
      uint8_t desp = p->entradas - 1 - j;
      p->X[i * p->entradas + j] = (double)((i >> desp) & 1U);
    }
  }
}

void cargar_salidas(perceptron_t *p, op_salida_e opcion, const char *secuencia)
{
  if (p == NULL) return;

  if (opcion == AND) {
    for (uint8_t i = 0; i < p->patrones; i++) {
      // 1 solo en el ultimo
      p->d_bin[i] = (i == p->patrones - 1) ? 1 : 0;
    }
  }

  else if (opcion == OR) {
    for (uint8_t i = 0; i < p->patrones; i++) {
      // 0 solo en el primero
      p->d_bin[i] = (i == 0) ? 0 : 1;
    }
  }

  else if (opcion == OTRO) {
    // Secuencia personalizada
    if (secuencia != NULL && strlen(secuencia) == p->patrones) {
      for (uint8_t i = 0; i < p->patrones; i++) {
        p->d_bin[i] = (secuencia[i] == '1') ? 1 : 0;
      }
    }
  }

  // Salida binaria a continua
  // para que LMS pueda calcular el error
  for (uint8_t i = 0; i < p->patrones; i++) {
    p->d[i] = (p->d_bin[i] == 0) ? -1.0 : 1.0;
  }
}

uint8_t contar_aciertos(const perceptron_t *p, const double *w_actual, double b_actual)
{
  if (p == NULL || w_actual == NULL) return 0;

  uint8_t aciertos = 0;

  for (uint8_t i = 0; i < p->patrones; i++) {
    double y = producto_punto(w_actual, &p->X[i * p->entradas], p->entradas) + b_actual;
    uint8_t salida = (y >= 0.0) ? 1 : 0;

    if (salida == p->d_bin[i]) aciertos++;
  }

  return aciertos;
}

uint8_t modelo_inferir(const modelo_t *m, const double *x_in)
{
  if (m == NULL || x_in == NULL || !m->entrenado) return 0;

  double lineal = producto_punto(m->w, x_in, m->entradas) + m->b;
  return (lineal >= 0) ? 1 : 0;
}

void entrenarLMS(perceptron_t *p, modelo_t *m)
{
  if (p == NULL || m == NULL) return;

  // Identificar clase minoritaria para balanceo
  uint8_t cont_cero = 0, cont_uno = 0;

  for (uint8_t i = 0; i < p->patrones; i++) {
    if (p->d_bin[i] == 0) cont_cero++;
    else cont_uno++;
  }

  uint8_t cl_min = (cont_cero <= cont_uno) ? 0 : 1;

  // Variables para elegir la mejor config
  int mejor_aciertos = -1;
  double mejor_mse = 1e300;
  double mejor_b = 0.0;
  uint16_t mejor_epocas = 0;
  bool mejor_convergio = false;
  bool mejor_divergio = false;

  double *mejor_w = (double *)malloc((size_t)p->entradas * sizeof(double));
  double *grad_w = (double *)calloc((size_t)p->entradas, sizeof(double));

  if (mejor_w == NULL || grad_w == NULL) {
    free(mejor_w);
    free(grad_w);
    return;
  }

  // Buscar mejor factor de ponderacion
  // Factor maximo de ponderacion
  double factor_max = 1.0;
  if (cont_cero > 0 && cont_uno > 0) {
    if (cl_min == 0) {
      factor_max = 2.0 * (double)cont_uno / (double)cont_cero;
    }

    else {
      factor_max = 2.0 * (double)cont_cero / (double)cont_uno;
    }
  }

  double factor = 1.0;
  uint8_t intento = 0;

  while (factor <= factor_max + TOLERANCIA) {
    uint8_t cantidad_min = (cl_min == 0) ? cont_cero : cont_uno;
    uint8_t cantidad_may = p->patrones - cantidad_min;
    double escala = 1.0;

    if (cantidad_min > 0 && cantidad_may > 0) {
      double suma_pesos = (factor * cantidad_min) + cantidad_may;
      escala = (double)p->patrones / suma_pesos;
    }

    // Inicializar pesos y bias para la iteracion actual
    for (uint8_t j = 0; j < p->entradas; j++) {
      p->w[j] = 0.0;
    }

    double b_actual = 0.0;
    double mse_anterior = 0.0, mse_actual = 0.0;
    bool convergio_actual = false, divergio_actual = false;
    uint16_t epocas_actuales = 0;
    uint8_t cont_estable = 0;

    // for por epocas
    for (uint16_t epoc = 0; epoc < MAX_EPOCAS; epoc++) {
      double grad_b = 0.0;
      double err_epoca = 0.0;

      for (uint8_t j = 0; j < p->entradas; j++) {
        grad_w[j] = 0.0;
      }

      for (uint8_t i = 0; i < p->patrones; i++) {
        double peso_muestra = 1.0;

        if (cantidad_min > 0 && cantidad_may > 0) {
          if (p->d_bin[i] == cl_min) peso_muestra = factor * escala;
          else peso_muestra = escala;
        }

        double y = producto_punto(p->w, &p->X[i * p->entradas], p->entradas) + b_actual;
        double e = p->d[i] - y;   // Error continuo

        // Acumulacion del gradiente
        for (uint8_t j = 0; j < p->entradas; j++) {
          grad_w[j] += peso_muestra * e * p->X[i * p->entradas + j];
        }

        grad_b += peso_muestra * e;
        err_epoca += peso_muestra * e * e;
      }

      // Actualizar pesos y bias
      for (uint8_t j = 0; j < p->entradas; j++) {
        p->w[j] += p->eta * grad_w[j] / (double)p->patrones;
      }

      b_actual += p->eta * grad_b / (double)p->patrones;
      mse_actual = err_epoca / (double)p->patrones;
      epocas_actuales = epoc + 1;

      // Verificar divergencia
      if (mse_actual != mse_actual || mse_actual > 1e100) {
        divergio_actual = true;
        break;
      }

      // Verificar convergencia
      if (epoc > 0) {
        if (fabs(mse_actual - mse_anterior) < TOLERANCIA) {
          cont_estable++;
        }
        else cont_estable = 0;

        if (cont_estable >= PACIENCIA) {
          convergio_actual = true;
          break;
        }
      }

      mse_anterior = mse_actual;
    }

    // Contar aciertos
    uint8_t aciertos_actuales = contar_aciertos(p, p->w, b_actual);

    // Conservar si mejora
    if (aciertos_actuales > mejor_aciertos || (aciertos_actuales == mejor_aciertos && mse_actual < mejor_mse)) {
      mejor_aciertos = aciertos_actuales;
      mejor_mse = mse_actual;
      mejor_b = b_actual;
      mejor_epocas = epocas_actuales;
      mejor_convergio = convergio_actual;
      mejor_divergio = divergio_actual;

      for (uint8_t j = 0; j < p->entradas; j++) {
        mejor_w[j] = p->w[j];
      }
    }

    intento++;

    if (factor_max == 1.0 || intento > 30) break;

    factor *= 2.0;
    if (factor > factor_max && (factor / 2.0) < factor_max) {
      factor = factor_max;
    }
  }

  // Guardar metricas finales
  p->aciertos = (uint8_t)mejor_aciertos;
  p->mse_final = mejor_mse;
  p->b = mejor_b;
  p->epocas_realizadas = mejor_epocas;
  p->convergio = mejor_convergio;
  p->divergio = mejor_divergio;

  for (uint8_t j = 0; j < p->entradas; j++) {
    p->w[j] = mejor_w[j];
  }

  // Transferir pesos finales a m
  m->b = mejor_b;
  m->entradas = p->entradas;

  for (uint8_t j = 0; j < p->entradas; j++) {
    m->w[j] = mejor_w[j];
  }

  m->entrenado = true;

  free(mejor_w);
  free(grad_w);
}