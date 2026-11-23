#include "errors.h"
#include "matrix_types.h"
#include <stdio.h>

void print_error_msg(rc_t exit_code)
{
    switch (exit_code)
    {
    case STATUS_MENU_CHOICE_ERR:
        printf("Ошибка: неправильный выбор пункта меню\n");
        break;
    case STATUS_STRING_INPUT_ERROR:
        printf("Ошибка: неверный строковый/числовой ввод\n");
        break;
    case STATUS_MEMORY_ALLOC_ERROR:
        printf("Ошибка: не удалось выделить память\n");
        break;
    case STATUS_DIM_MISMATCH_ERROR:
        printf("Ошибка: несовместимые размеры матриц (A.cols != B.rows)\n");
        break;
    case STATUS_MATRIX_NOT_SET:
        printf("Ошибка: матрица не задана\n");
        break;
    case STATUS_EMPTY_INPUT_ERROR:
        printf("Ошибка: пустой ввод — ни одного ненулевого элемента не задано\n");
        break;
    case STATUS_INVALID_INDEX_ERROR:
        printf("Ошибка: индекс вне допустимого диапазона\n");
        break;
    case STATUS_INVALID_DENSITY_ERROR:
        printf("Ошибка: плотность должна быть в диапазоне [0.0, 1.0]\n");
        break;
    case STATUS_INVALID_DIMENSION_ERROR:
        printf("Ошибка: размерность матрицы должна быть в диапазоне 1..%d\n", MAX_DIM);
        break;
    case STATUS_MATRIX_TOO_LARGE:
        printf("Ошибка: матрица слишком большая для плотного представления\n");
        break;
    case STATUS_INPUT_EOF:
        printf("Ввод завершён\n");
        break;
    default:
        printf("Неизвестный код ошибки: %d\n", (int)exit_code);
        break;
    }
}
