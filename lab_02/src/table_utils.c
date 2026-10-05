#include <stdio.h>
#include "table_utils.h"
#include "table_io.h"
#include <string.h>

static rc_t add_string_field(char *str, size_t max_size)
{
    rc_t rc = STATUS_NO_ERROR;
    if (str == NULL || max_size == 0)
        rc = STATUS_NULL_POINTER_ERR;

    if (rc == STATUS_NO_ERROR && fgets(str, (int)max_size, stdin) == NULL)
        rc = STATUS_STRING_INPUT_ERROR;

    if (rc == STATUS_NO_ERROR && strchr(str, '\n') == NULL && !feof(stdin))
    {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) { }
        rc = STATUS_FIELD_OVERFLOW;
    }

    if (rc == STATUS_NO_ERROR)
        str[strcspn(str, "\n")] = '\0';

    return rc;
}

static rc_t add_int_field(int *n)
{
    rc_t rc = STATUS_NO_ERROR;
    char buf[32];
    if (fgets(buf, sizeof buf, stdin) == NULL)
        rc = STATUS_STRING_INPUT_ERROR;
    if (rc == STATUS_NO_ERROR && sscanf(buf, "%d", n) != 1)
        rc = STATUS_TOKEN_ERROR;
    return rc;
}

static rc_t add_dbl_field(double *d)
{
    rc_t rc = STATUS_NO_ERROR;
    char buf[32];
    if (fgets(buf, sizeof buf, stdin) == NULL)
        rc = STATUS_STRING_INPUT_ERROR;
    if (rc == STATUS_NO_ERROR && sscanf(buf, "%lf", d) != 1)
        rc = STATUS_TOKEN_ERROR;
    return rc;
}

rc_t add_record(Record_t *records, size_t *rec_len)
{
    rc_t rc = STATUS_NO_ERROR;

    if (records == NULL || rec_len == NULL)
        rc = STATUS_NULL_POINTER_ERR;

    if (*rec_len >= MAX_RECORD_AMOUNT)
    {
        printf("Невозможна запись, кол-во записей >= %d\n", MAX_RECORD_AMOUNT);
        rc = STATUS_RECORD_OVERFLOW;
    }

    Record_t *r = &records[*rec_len];
    char kind_buf[MAX_KIND_NAME];

    if (rc == STATUS_NO_ERROR)
    {
        printf("Введите страну: ");
        rc = add_string_field(r->country, MAX_COUNTRY_NAME);
    }
    if (rc == STATUS_NO_ERROR)
    {
        printf("Введите столицу: ");
        rc = add_string_field(r->capital, MAX_CAPITAL_NAME);
    }
    if (rc == STATUS_NO_ERROR)
    {
        printf("Введите материк: ");
        rc = add_string_field(r->continent, MAX_CONTINENT_NAME);
    }
    if (rc == STATUS_NO_ERROR)
    {
        printf("Требуется ли виза (да/нет): ");
        rc = add_string_field(r->visa, MAX_VISA_NAME);
        if (rc == STATUS_NO_ERROR && strcmp(r->visa, "да") != 0 && strcmp(r->visa, "нет") != 0)
            rc = STATUS_VISA_INP_ERROR;
    }
    if (rc == STATUS_NO_ERROR)
    {
        printf("Время полета: ");
        rc = add_dbl_field(&r->travel_time);
    }
    if (rc == STATUS_NO_ERROR)
    {
        printf("Введите цену: ");
        rc = add_dbl_field(&r->leisure_cost);
    }
    if (rc == STATUS_NO_ERROR)
    {
        printf("Введите вид туризма "
               "(Экскурсионный/Пляжный/Спортивный): ");
        rc = add_string_field(kind_buf, MAX_KIND_NAME);
    }

    if (rc == STATUS_NO_ERROR)
    {
        if (strcmp("Экскурсионный", kind_buf) == 0)
        {
            r->kind = SIGHTSEEING;

            printf("Введите количество объектов: ");
            rc = add_int_field(&r->tourism.sightseeing_obj.object_count);

            if (rc == STATUS_NO_ERROR)
            {
                printf("Введите основной объект: ");
                rc = add_string_field(r->tourism.sightseeing_obj.main_obj,
                                      MAX_OBJECT_NAME);
            }
        }
        else if (strcmp("Пляжный", kind_buf) == 0)
        {
            r->kind = BEACH;

            printf("Введите сезон: ");
            rc = add_string_field(r->tourism.beach_obj.main_season,
                                  MAX_MAIN_SEASON_NAME);

            if (rc == STATUS_NO_ERROR)
            {
                printf("Введите температуру воздуха: ");
                rc = add_int_field(&r->tourism.beach_obj.air_temperature);
            }
            if (rc == STATUS_NO_ERROR)
            {
                printf("Введите температуру воды: ");
                rc = add_int_field(&r->tourism.beach_obj.water_temperature);
            }
        }
        else if (strcmp("Спортивный", kind_buf) == 0)
        {
            r->kind = SPORT;

            printf("Введите вид спорта: ");
            rc = add_string_field(r->tourism.sport_obj.sport_type,
                                  MAX_SPORT_TYPE_NAME);
            if (rc == STATUS_NO_ERROR && strcmp(r->tourism.sport_obj.sport_type, "горные лыжи") != 0
                && strcmp(r->tourism.sport_obj.sport_type, "восхождения") != 0 
                && strcmp(r->tourism.sport_obj.sport_type, "серфинг") != 0)
                rc = STATUS_SPORT_INP_ERR;
        }
        else
        {
            rc = STATUS_KIND_FIELD_ERROR;
        }
    }

    if (rc == STATUS_NO_ERROR)
        (*rec_len)++;

    return rc;
}

rc_t delete_records_by_country(Record_t *records, size_t *rec_len)
{
    rc_t rc = STATUS_NO_ERROR;

    if (records == NULL || rec_len == NULL)
        rc = STATUS_NULL_POINTER_ERR;

    if (rc == STATUS_NO_ERROR && *rec_len == 0)
        rc = STATUS_NOT_FOUND_ERROR;

    char country[MAX_COUNTRY_NAME];

    if (rc == STATUS_NO_ERROR)
    {
        printf("Введите страну для удаления: ");
        rc = add_string_field(country, MAX_COUNTRY_NAME);
    }

    if (rc == STATUS_NO_ERROR)
    {
        size_t found = 0;
        size_t i = 0;

        while (i < *rec_len)
        {
            if (strcmp(records[i].country, country) == 0)
            {
                if (i + 1 < *rec_len)
                    memmove(&records[i], &records[i + 1], (*rec_len - i - 1) * sizeof(Record_t));
                (*rec_len)--;
                found++;
            }
            else
            {
                i++;
            }
        }

        if (found == 0)
            rc = STATUS_NOT_FOUND_ERROR;
    }

    return rc;
}

rc_t process_continent_field(Record_t *records, size_t rec_len)
{
    rc_t rc = STATUS_NO_ERROR;
    const char continents[7][MAX_CONTINENT_NAME] = {"Европа", "Азия", "Северная Америка", "Южная Америка", "Африка", "Австралия", "Антарктика"};
    const char sport_type[3][MAX_SPORT_TYPE_NAME] = {"горные лыжи", "серфинг", "восхождения"};

    req_t request;

    int valid = 0;

    printf("Введите материк: ");
    rc = add_string_field(request.requested_continent, MAX_CONTINENT_NAME);
    if (rc == STATUS_NO_ERROR)
    {
        for (int i = 0; i < 7 && !valid; i++)
        {
            if (strncmp(continents[i], request.requested_continent, MAX_CONTINENT_NAME) == 0)
                valid = 1;
        }
    }
    if (rc == STATUS_NO_ERROR && !valid)
        rc = STATUS_CONTINENT_INP_ERR;
    if (rc == STATUS_NO_ERROR)
    {
        valid = 0;
        printf("Введите интересующий вас вид спорта: ");
        rc = add_string_field(request.requested_sport_type, MAX_SPORT_TYPE_NAME);
    }
    if (rc == STATUS_NO_ERROR)
    {
        for (int i = 0; i < 3 && !valid; i++)
        {
            if (strncmp(sport_type[i], request.requested_sport_type, MAX_SPORT_TYPE_NAME) == 0)
                valid = 1;
        }
    }
    if (rc == STATUS_NO_ERROR && !valid)
        rc = STATUS_SPORT_INP_ERR;
    if (rc == STATUS_NO_ERROR)
    {
        printf("Укажите максимальную стоимость отдыха: ");
        if (add_dbl_field(&(request.requested_price)) != STATUS_NO_ERROR)
            rc = STATUS_PRICE_INP_ERR;
    }
    if (rc == STATUS_NO_ERROR)
    {
        print_requested_fields(records, rec_len, &request);
    }
    return rc;
}
