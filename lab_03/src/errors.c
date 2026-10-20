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
    case STATUS_CHOICE_LIMIT_ERR:
        printf("Ошибка: выбор вне допустимого диапазона\n");
        break;
    case STATUS_FILE_OPEN_ERROR:
        printf("Ошибка: не удалось открыть файл. Проверьте путь и наличие файла\n");
        break;
    case STATUS_FILE_READ_ERROR:
        printf("Ошибка: не удалось прочитать данные из файла\n");
        break;
    case STATUS_FILE_WRITE_ERROR:
        printf("Ошибка: не удалось записать данные в файл\n");
        break;
    case STATUS_STRING_INPUT_ERROR:
        printf("Ошибка: неверный строковый/числовой ввод\n");
        break;
    case STATUS_NULL_POINTER_ERR:
        printf("Internal error: обнаружен NULL-указатель\n");
        break;
    case STATUS_MEMORY_ALLOC_ERROR:
        printf("Ошибка: не удалось выделить память\n");
        break;
    case STATUS_RANGE_ERROR:
        printf("Ошибка: значение вне допустимого диапазона\n");
        break;
    case STATUS_DIM_MISMATCH_ERROR:
        printf("Ошибка: несовместимые размеры матриц (A.cols != B.rows)\n");
        break;
    case STATUS_MATRIX_NOT_SET:
        printf("Ошибка: матрица не задана\n");
        break;
    case STATUS_VECTOR_NOT_SET:
        printf("Ошибка: вектор не задан\n");
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
    case STATUS_CSR_STRUCTURE_ERROR:
        printf("Ошибка: нарушена структура CSR-матрицы\n");
        break;
    case STATUS_CSC_STRUCTURE_ERROR:
        printf("Ошибка: нарушена структура CSC-матрицы\n");
        break;
    case STATUS_OPERATION_ERROR:
        printf("Ошибка: не удалось выполнить операцию\n");
        break;
    case STATUS_MATRIX_TOO_LARGE:
        printf("Ошибка: матрица слишком большая для плотного представления\n");
        break;
    case STATUS_FILENAME_OVERFLOW:
        printf("Ошибка: слишком длинное имя файла\n");
        break;
    case STATUS_GENERATION_ERROR:
        printf("Ошибка: не удалось сгенерировать матрицу\n");
        break;
    default:
        printf("Неизвестный код ошибки: %d\n", (int)exit_code);
        break;
    }
}
