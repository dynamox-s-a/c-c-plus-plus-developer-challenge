# Operations

Descrição
- Componente que concentra as operações matemáticas usadas pela HMI.
- Atualmente implementa soma de dois valores e cálculo de determinante de matriz quadrada.

Objetivo
- Expor funções utilitárias com retorno `esp_err_t` para integração simples com ESP-IDF.

Estrutura
- `operations.c` — implementação das funções matemáticas.
- `include/operations.h` — API pública do componente.

Integração
- Consumido pela HMI para executar cálculos selecionados no menu interativo.
- Retornos `esp_err_t` são propagados para diagnóstico no `main`.

API pública (atual)
- `esp_err_t operations_add(double lhs, double rhs, double *result);`
: Soma `lhs + rhs` e escreve em `result`.
- `esp_err_t operations_matrix_determinant(const double *matrix, size_t dimension, double *result);`
: Calcula o determinante de uma matriz quadrada `dimension x dimension` em formato linear (row-major).

Parâmetros e validações
- `result` deve ser ponteiro válido.
- `matrix` deve ser ponteiro válido para `dimension * dimension` elementos.
- `dimension` deve respeitar os limites usados pela aplicação (na HMI atual: 1 a 6).
- Em caso de parâmetro inválido, as funções retornam erro (`ESP_ERR_INVALID_ARG` ou equivalente).

Exemplo de uso (C, ESP-IDF)
```c
#include "operations.h"

void example_add(void)
{
	double result = 0.0;
	if (operations_add(2.5, 4.0, &result) == ESP_OK) {
		// result = 6.5
	}
}

void example_determinant(void)
{
	// Matriz 2x2: [1 2; 3 4]
	const double matrix[] = {1.0, 2.0,
							 3.0, 4.0};
	double det = 0.0;

	if (operations_matrix_determinant(matrix, 2, &det) == ESP_OK) {
		// det = -2
	}
}
```

Testes
- Validar soma com casos positivos, negativos e ponto flutuante.
- Validar determinante para matrizes 1x1, 2x2 e 3x3 conhecidas.
- Validar tratamento de argumentos inválidos (`NULL`, dimensão inválida).

Notas
- O componente é stateless (sem estado global exposto), facilitando reuso e testes.



