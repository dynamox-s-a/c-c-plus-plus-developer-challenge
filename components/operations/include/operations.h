#pragma once

#include <stddef.h>

#include "esp_err.h"

esp_err_t operations_add(double lhs, double rhs, double *result); // Adiciona dois valores e armazena o resultado em 'result'. Retorna ESP_OK em caso de sucesso ou um código de erro apropriado.
esp_err_t operations_matrix_determinant(const double *matrix, size_t dimension, double *result); // Calcula o determinante de uma matriz quadrada de dimensão 'dimension' e armazena o resultado em 'result'. Retorna ESP_OK em caso de sucesso ou um código de erro apropriado.