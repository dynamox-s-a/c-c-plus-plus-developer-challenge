#include "hmi.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_check.h"
#include "esp_log.h"

#include "operations.h"

#define HMI_MAX_OPERATIONS 8U

typedef esp_err_t (*operation_handler_t)(void);

typedef struct
{
    const char *name;
    const char *description;
    operation_handler_t handler;
} hmi_operation_t;

static const char *TAG = "hmi";

static hmi_operation_t s_operations[HMI_MAX_OPERATIONS];
static size_t s_operation_count;
static bool s_running;

static void trim_whitespace(char *text)
{
    if (text == NULL)
    {
        return;
    }

    char *start = text;
    while (*start != '\0' && isspace((unsigned char)*start))
    {
        ++start;
    }

    char *end = text + strlen(text);
    while (end > start && isspace((unsigned char)*(end - 1)))
    {
        --end;
    }

    size_t length = (size_t)(end - start);
    if (start != text)
    {
        memmove(text, start, length);
    }
    text[length] = '\0';
}

static esp_err_t hmi_write(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int printed = vprintf(format, args);
    va_end(args);
    fflush(stdout);
    return (printed < 0) ? ESP_FAIL : ESP_OK;
}

static esp_err_t hmi_read_line(char *buffer, size_t buffer_length)
{
    if (buffer == NULL || buffer_length == 0U)
    {
        return ESP_ERR_INVALID_ARG;
    }

    size_t length = 0U;
    while (true)
    {
        int ch = fgetc(stdin);
        if (ch == EOF)
        {
            return ESP_FAIL;
        }

        if (ch == '\r' || ch == '\n')
        {
            // Keep prompt behavior intuitive when terminal has no local echo.
            ESP_RETURN_ON_ERROR(hmi_write("\n"), TAG, "Falha ao imprimir quebra de linha");
            break;
        }

        if (ch == '\b' || ch == 0x7F)
        {
            if (length > 0U)
            {
                --length;
                ESP_RETURN_ON_ERROR(hmi_write("\b \b"), TAG, "Falha ao ecoar backspace");
            }
            continue;
        }

        if (!isprint((unsigned char)ch))
        {
            continue;
        }

        if (length < (buffer_length - 1U))
        {
            buffer[length++] = (char)ch;
            ESP_RETURN_ON_ERROR(hmi_write("%c", (char)ch), TAG, "Falha ao ecoar caractere");
        }
    }

    buffer[length] = '\0';

    trim_whitespace(buffer);
    return ESP_OK;
}

static esp_err_t prompt_double(const char *prompt, double *value)
{
    if (prompt == NULL || value == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    char input[64];
    while (true)
    {
        ESP_RETURN_ON_ERROR(hmi_write("%s", prompt), TAG, "Falha ao imprimir prompt");
        ESP_RETURN_ON_ERROR(hmi_read_line(input, sizeof(input)), TAG, "Falha ao ler entrada");

        char *end = NULL;
        double parsed = strtod(input, &end);
        if (end == input)
        {
            ESP_RETURN_ON_ERROR(hmi_write("Entrada invalida. Tente novamente.\n"), TAG, "Falha ao imprimir erro");
            continue;
        }

        while (end != NULL && *end != '\0' && isspace((unsigned char)*end))
        {
            ++end;
        }

        if (end != NULL && *end != '\0')
        {
            ESP_RETURN_ON_ERROR(hmi_write("Entrada invalida. Tente novamente.\n"), TAG, "Falha ao imprimir erro");
            continue;
        }

        *value = parsed;
        return ESP_OK;
    }
}

static esp_err_t prompt_uint_range(const char *prompt, size_t min_value, size_t max_value, size_t *value)
{
    if (prompt == NULL || value == NULL || min_value > max_value)
    {
        return ESP_ERR_INVALID_ARG;
    }

    char input[64];
    while (true)
    {
        ESP_RETURN_ON_ERROR(hmi_write("%s", prompt), TAG, "Falha ao imprimir prompt");
        ESP_RETURN_ON_ERROR(hmi_read_line(input, sizeof(input)), TAG, "Falha ao ler entrada");

        char *end = NULL;
        long parsed = strtol(input, &end, 10);
        if (end == input || parsed < 0)
        {
            ESP_RETURN_ON_ERROR(hmi_write("Entrada invalida. Tente novamente.\n"), TAG, "Falha ao imprimir erro");
            continue;
        }

        while (end != NULL && *end != '\0' && isspace((unsigned char)*end))
        {
            ++end;
        }

        if (end == NULL || *end != '\0' || parsed < (long)min_value || parsed > (long)max_value)
        {
            ESP_RETURN_ON_ERROR(hmi_write("Valor fora do intervalo permitido. Tente novamente.\n"), TAG, "Falha ao imprimir erro");
            continue;
        }

        *value = (size_t)parsed;
        return ESP_OK;
    }
}

static esp_err_t register_operation(const hmi_operation_t *operation)
{
    if (operation == NULL || operation->name == NULL || operation->description == NULL || operation->handler == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_operation_count >= HMI_MAX_OPERATIONS)
    {
        return ESP_ERR_NO_MEM;
    }

    s_operations[s_operation_count++] = *operation;
    return ESP_OK;
}

static esp_err_t sum_operation(void)
{
    double lhs = 0.0;
    double rhs = 0.0;
    double result = 0.0;

    ESP_RETURN_ON_ERROR(prompt_double("Digite o primeiro valor: ", &lhs), TAG, "Falha na leitura do primeiro valor");
    ESP_RETURN_ON_ERROR(prompt_double("Digite o segundo valor: ", &rhs), TAG, "Falha na leitura do segundo valor");
    ESP_RETURN_ON_ERROR(operations_add(lhs, rhs, &result), TAG, "Falha ao calcular soma");

    ESP_RETURN_ON_ERROR(hmi_write("Resultado: %.6f + %.6f = %.6f\n", lhs, rhs, result), TAG, "Falha ao imprimir resultado");
    return ESP_OK;
}

static esp_err_t determinant_operation(void)
{
    size_t dimension = 0U;
    ESP_RETURN_ON_ERROR(prompt_uint_range("Digite a dimensao da matriz NxN (1 a 6): ", 1U, 6U, &dimension), TAG, "Falha na leitura da dimensao");

    const size_t element_count = dimension * dimension;
    double *matrix = calloc(element_count, sizeof(double));
    if (matrix == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    for (size_t row = 0U; row < dimension; ++row)
    {
        for (size_t column = 0U; column < dimension; ++column)
        {
            char prompt[64];
            snprintf(prompt, sizeof(prompt), "Elemento [%u][%u]: ", (unsigned)(row + 1U), (unsigned)(column + 1U));
            ESP_RETURN_ON_ERROR(prompt_double(prompt, &matrix[row * dimension + column]), TAG, "Falha ao ler elemento da matriz");
        }
    }

    double result = 0.0;
    esp_err_t err = operations_matrix_determinant(matrix, dimension, &result);
    free(matrix);
    ESP_RETURN_ON_ERROR(err, TAG, "Falha ao calcular determinante");

    ESP_RETURN_ON_ERROR(hmi_write("Determinante da matriz %ux%u = %.6f\n", (unsigned)dimension, (unsigned)dimension, result), TAG, "Falha ao imprimir resultado");
    return ESP_OK;
}

static esp_err_t register_default_operations(void)
{
    static const hmi_operation_t default_operations[] = {
        {.name = "Soma", .description = "Soma de dois valores", .handler = sum_operation},
        {.name = "Determinante", .description = "Calcula o determinante de uma matriz NxN", .handler = determinant_operation},
    };

    for (size_t index = 0U; index < sizeof(default_operations) / sizeof(default_operations[0]); ++index)
    {
        ESP_RETURN_ON_ERROR(register_operation(&default_operations[index]), TAG, "Falha ao registrar operacao padrao");
    }

    return ESP_OK;
}

static esp_err_t print_menu(void)
{
    ESP_RETURN_ON_ERROR(hmi_write("\n=== Dynamox HMI ===\n"), TAG, "Falha ao imprimir menu");
    for (size_t index = 0U; index < s_operation_count; ++index)
    {
        ESP_RETURN_ON_ERROR(hmi_write("%u - %s (%s)\n", (unsigned)(index + 1U), s_operations[index].name, s_operations[index].description), TAG, "Falha ao imprimir operacao");
    }
    ESP_RETURN_ON_ERROR(hmi_write("0 - Sair\n"), TAG, "Falha ao imprimir opcao de saida");
    return ESP_OK;
}

esp_err_t hmi_run(void)
{
    s_running = true;
    s_operation_count = 0U;

    esp_err_t err = register_default_operations();
    if (err != ESP_OK)
    {
        return err;
    }

    ESP_RETURN_ON_ERROR(hmi_write("\nDynamox C/C++ Developer Challenge\n"), TAG, "Falha ao imprimir cabecalho");
    ESP_RETURN_ON_ERROR(hmi_write("USB-C unico: comandos e respostas na mesma porta.\n"), TAG, "Falha ao imprimir informacao da porta");
    ESP_RETURN_ON_ERROR(hmi_write("Digite o numero da operacao e pressione Enter.\n"), TAG, "Falha ao imprimir instrucoes");

    while (s_running)
    {
        size_t option = 0U;
        ESP_RETURN_ON_ERROR(print_menu(), TAG, "Falha ao imprimir menu");
        ESP_RETURN_ON_ERROR(prompt_uint_range("Opcao: ", 0U, s_operation_count, &option), TAG, "Falha na leitura da opcao");

        if (option == 0U)
        {
            s_running = false;
            ESP_RETURN_ON_ERROR(hmi_write("Encerrando aplicacao.\n"), TAG, "Falha ao imprimir encerramento");
            break;
        }

        if (option > s_operation_count)
        {
            ESP_RETURN_ON_ERROR(hmi_write("Opcao invalida.\n"), TAG, "Falha ao imprimir erro");
            continue;
        }

        esp_err_t operation_err = s_operations[option - 1U].handler();
        if (operation_err != ESP_OK)
        {
            ESP_RETURN_ON_ERROR(hmi_write("Operacao falhou: %s\n", esp_err_to_name(operation_err)), TAG, "Falha ao imprimir erro da operacao");
        }
    }

    return ESP_OK;
}