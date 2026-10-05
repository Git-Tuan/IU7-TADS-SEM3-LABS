#include "errors.h"
#include "defines.h"
#include <stdio.h>


void print_error_msg(rc_t exit_code)
{
    switch(exit_code)
    {
    case STATUS_MENU_CHOICE_ERR:
        printf("Ошибка: неправильный выбор\n");
        break;
    case STATUS_CHOICE_LIMIT_ERROR:
        printf("Ошибка: введите число от 0 до %d включительно\n", MAX_NUMBER_OF_CHOICE);
        break; 
    case STATUS_FILE_OPEN_ERROR:
        printf("Ошибка: указанный файл невозможно открыть. Проверьте путь к файлу и существует ли сам файл\n");
        break;
    case STATUS_STRING_INPUT_ERROR:
        printf("Ошибка: неправильный ввод\n");
        break;
    case STATUS_NULL_POINTER_ERR:
        printf("Internal error: NULL pointer found\n");
        break;
    case STATUS_TOKEN_ERROR:
        printf("Ошибка: разделители полей должны быть только символы \"|\" \n");
        break;
    case STATUS_KIND_FIELD_ERROR:
        printf("Ошибка: неверно указан тип туризма. Возможные виды туризма: Экскурсионный, Спортивный, Пляжный (регистрозависим)\n");
        break;
    case STATUS_RECORD_OVERFLOW:
        printf("Ошибка: превышение количества записей. Максимально возможное: %d\n", MAX_RECORD_AMOUNT);
        break;
    case STATUS_NOT_FOUND_ERROR:
        printf("Ошибка: не удалось найти страны по указанной характеристике\n");
        break;
    case STATUS_SPORT_INP_ERR:
        printf("Ошибка: неправильный формат ввода вида спорта. Возможные виды спорта: горные лыжи, серфинг, восхождения (регистрозависим)\n");
        break;
    case STATUS_CONTINENT_INP_ERR:
        printf("Ошибка: неправильный формат ввода континента. Континенты: Европа, Азия, Африка, Австралия, Антарктика, Южная Америка, Северная Америка\n");
        break;
    case STATUS_PRICE_INP_ERR:
        printf("Ошибка: неправильный ввод цены отдыха\n");
        break;
    case STATUS_FILE_WRITE_ERROR:
        printf("Ошибка: неправильный формат ввода при записи в файл\n");
        break;
    case STATUS_FILENAME_OVERFLOW:
        printf("Ошибка: слишком длинный путь к файлу. Максимально возможное количество символов - %d\n", MAX_FILENAME - 2);
        break;
    case STATUS_VISA_INP_ERROR:
        printf("Ошибка: в поле визы нужно ввести да/нет\n");
        break;
    case STATUS_FIELD_OVERFLOW:
        printf("Ошибка: слишком длинное значение поля\n");
        break;

    default:
        break;
    }
}