#include <stdio.h>
#include "file_utils.h"
#include "table_core.h"
#include "table_io.h"
#include "table_utils.h"
#include "measure_sorts.h"
#include "sorts.h"

static rc_t handle_user_choice(menu_item_t choice, Record_t *records, size_t *rec_len, int *keys)
{
    rc_t rc = STATUS_NO_ERROR;
    switch (choice)
    {
    case MENU_EXIT:
        printf("Exiting...\n");
        break;

    case MENU_ADD:
        rc = add_record(records, rec_len);
        if (rc == STATUS_NO_ERROR) init_keys(keys, *rec_len);
        break;

    case MENU_DELETE_BY_NAME:
        rc = delete_records_by_country(records, rec_len);
        if (rc == STATUS_NO_ERROR) init_keys(keys, *rec_len);
        break;
    case MENU_PRINT_TABLE:
        print_table(records, *rec_len);
        break;

    case MENU_SELECTION_SORT_TABLE:
        table_selection_sort(records, *rec_len);
        init_keys(keys, *rec_len);
        printf("Таблица отсортирована выборной сортировкой\n");
        break;

    case MENU_QUICK_SORT_TABLE:
        table_quicksort(records, 0, *rec_len - 1);
        init_keys(keys, *rec_len);
        printf("Таблица отсортирована быстрой сортировкой\n");
        break;

    case MENU_KEYS_SELECTION_SORT:
        init_keys(keys, *rec_len);
        key_selection_sort(records, keys, *rec_len);
        printf("Таблица отсортирована выборной сортировкой ключами\n");
        break;

    case MENU_KEYS_QUICK_SORT:
        init_keys(keys, *rec_len);
        keys_quicksort(records, keys, 0, *rec_len - 1);
        printf("Таблица отсортирована быстрой сортировкой ключами\n");
        break;

    case MENU_PRINT_KEYS:
        print_keys(keys, *rec_len);
        break;

    case MENU_FILTER_BY_CONT:
        rc = process_continent_field(records, *rec_len);
        break;

    case MENU_MEASURE_SORT_TIME:
        rc = measure_sorts(records, *rec_len);
        break;

    case MENU_PRINT_TABLE_VIA_KEYS:
        print_table_via_keys(records, keys, *rec_len);
        break;

    case MENU_SAVE_TABLE_TO_FILE:
        rc = save_table(records, *rec_len);
        if (rc == STATUS_NO_ERROR) printf("Данные успешно записаны в файл\n");
        break;

    default:
        break;
    }

    return rc;
}


rc_t menu(Record_t *records, size_t *rec_len)
{
    rc_t rc = STATUS_NO_ERROR;
    menu_item_t user_response;
    int temp_response, keys[MAX_RECORD_AMOUNT];
    init_keys(keys, *rec_len);
    printf("Формат ввода записи: название страны, столицу, материк,"
            "необходимость наличия визы, время полета до страны, минимальную"
            "стоимость отдыха, основной вид туризма\n");
    printf("Инфо: Сортировка происходит по стоимости отдыха\n");

    do {
        printf("0. Выход\n");
        printf("1. Добавить запись\n2. Удалить запись по названию страны\n3. Вывести таблицу\n");
        printf("4. Отсортировать таблицу записей выборной сортировкой\n");
        printf("5. Отсортировать таблицу записей быстрой сортировкой\n");
        printf("6. Отсортировать с помощью таблицы ключей выборной сортировкой\n");
        printf("7. Отсортировать с помощью таблицы ключей быстрой сортировкой \n");
        printf("8. Вывести таблицу ключей\n");
        printf("9. Вывести список стран на выбранном материке, где можно заняться указанным видом спорта, со стоимостью отдыха меньше указанной\n");
        printf("10. Измерить времена сортировок\n");
        printf("11. Вывести таблицу массивом ключей\n");
        printf("12. Сохранить таблицу в файл\n");
        
        rc = scanf("%d", &temp_response) == 1 ? rc : STATUS_MENU_CHOICE_ERR;
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        rc = temp_response <= MAX_NUMBER_OF_CHOICE && temp_response >= 0 ? rc : STATUS_CHOICE_LIMIT_ERROR;
        user_response = rc == STATUS_NO_ERROR ? (menu_item_t)temp_response : 100;
        rc = rc == STATUS_NO_ERROR ? handle_user_choice(user_response, records, rec_len, keys) : rc;
        
    } while (user_response != MENU_EXIT);

    return rc;
}
