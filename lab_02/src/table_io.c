#include "table_io.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void pad(const char *s, int w)
{
    if (!s) s = "";
    int len = 0;
    for (const unsigned char *p = (const unsigned char*)s; *p; p++)
        if ((*p & 0xC0) != 0x80) ++len;
    fputs(s, stdout);
    while (len++ < w) putchar(' ');
}

static rc_t get_field(char **token)
{
    rc_t rc = STATUS_NO_ERROR;
    (*token) = strtok(NULL, "|");
    if (!(*token))
    {
        printf("Error inside get_field\n");
        rc = STATUS_TOKEN_ERROR;
    }
    return rc;
}

static rc_t read_one_record(char *buffer, Record_t *record)
{
    rc_t rc = STATUS_NO_ERROR;

    char *token = strtok(buffer, "|");
    if (!token) 
    {
        rc = STATUS_TOKEN_ERROR;
        printf("Error in the 1st split\n");
    }
    if (rc == STATUS_NO_ERROR)
    {
        strcpy(record->country, token);
        rc = get_field(&token);
        if (rc == STATUS_NO_ERROR) strcpy(record->capital, token);
        rc = rc == STATUS_NO_ERROR ? get_field(&token) : rc;
        if (rc == STATUS_NO_ERROR) strcpy(record->continent, token);
        rc = rc == STATUS_NO_ERROR ? get_field(&token) : rc;
        if (rc == STATUS_NO_ERROR) strcpy(record->visa, token);
        rc = rc == STATUS_NO_ERROR ? get_field(&token) : rc;
        if (rc == STATUS_NO_ERROR)
        {
            rc = sscanf(token, "%lf", &record->travel_time) == 1 ? rc : STATUS_TOKEN_ERROR;
            if (rc != STATUS_NO_ERROR)
            {
                printf("Error in the travel_time\n");
                printf("%s\n", token);
            }
            rc = get_field(&token);
        }
        if (rc == STATUS_NO_ERROR)
        {

            rc = sscanf(token, "%lf", &record->leisure_cost) == 1 ? rc : STATUS_TOKEN_ERROR;
            if (rc != STATUS_NO_ERROR)
            {
                printf("Error in the leisure_cost\n");
                printf("%s\n", token);
            }
        }
    }

    if (rc == STATUS_NO_ERROR)
    {
        rc = get_field(&token);
        if (rc == STATUS_NO_ERROR)
        {
            if (strcmp("Экскурсионный", token) == 0)
            {
                record->kind = SIGHTSEEING;
                rc = get_field(&token);
                if (rc == STATUS_NO_ERROR)
                {
                    rc = sscanf(token, "%d", &(record->tourism.sightseeing_obj.object_count))
                                == 1 ? rc : STATUS_TOKEN_ERROR;
                    if (rc != STATUS_NO_ERROR)
                    {
                        printf("Error in the object_count\n");
                        printf("%s\n", token);
                    }
                }
                if (rc == STATUS_NO_ERROR)
                    rc = get_field(&token);
                if (rc == STATUS_NO_ERROR)
                {
                    strcpy((record->tourism.sightseeing_obj.main_obj), token);
                }
            }
            else if (strcmp("Пляжный", token) == 0)
            {
                record->kind = BEACH;
                rc = get_field(&token);
                if (rc == STATUS_NO_ERROR)
                {
                    strcpy((record->tourism.beach_obj.main_season), token);
                }
                if (rc == STATUS_NO_ERROR)
                    rc = get_field(&token);

                if (rc == STATUS_NO_ERROR)
                {
                    rc = sscanf(token, "%d", &(record->tourism.beach_obj.air_temperature))
                        == 1 ? rc : STATUS_TOKEN_ERROR;
                    if (rc != STATUS_NO_ERROR)
                    {
                        printf("Error in the air_temperature\n");
                        printf("%s\n", token);
                    }
                }
                
                if (rc == STATUS_NO_ERROR)
                    rc = get_field(&token);
                if (rc == STATUS_NO_ERROR)
                {
                    rc = sscanf(token, "%d", &(record->tourism.beach_obj.water_temperature))
                        == 1 ? rc : STATUS_TOKEN_ERROR;
                    if (rc != STATUS_NO_ERROR)
                    {
                        printf("Error in the water_temperature\n");
                        printf("%s\n", token);
                    }
                }
            }
            else if (strcmp("Спортивный", token) == 0)
            {
                record->kind = SPORT;
                rc = get_field(&token);
                if (rc == STATUS_NO_ERROR)
                {
                    strcpy((record->tourism.sport_obj.sport_type), token);
                }
            }
            else
                rc = STATUS_KIND_FIELD_ERROR;
        
        }

    }


    return rc;
}

rc_t read_filename(char *filename)
{
    rc_t rc = STATUS_NO_ERROR;

    printf("Введите путь к файлу: ");

    if (filename == NULL)
        rc = STATUS_NULL_POINTER_ERR;
    if (rc == STATUS_NO_ERROR && fgets(filename, MAX_FILENAME, stdin) == NULL)
        rc = STATUS_STRING_INPUT_ERROR;
    if (rc == STATUS_NO_ERROR && strchr(filename, '\n') == NULL)
        rc = STATUS_FILENAME_OVERFLOW;

    return rc;
}

rc_t read_records_from_file(const char *filename, Record_t *records, size_t *record_cnt)
{
    rc_t rc = STATUS_NO_ERROR;
    FILE *file = NULL;
    size_t line_cnt = 0;
    if (filename == NULL)
        rc = STATUS_NULL_POINTER_ERR;
    if (rc == STATUS_NO_ERROR)
    {
        file = fopen(filename, "r");
        rc = file == NULL ? STATUS_FILE_OPEN_ERROR : rc;
    }
    if (rc == STATUS_NO_ERROR)
    {
        const size_t buff_size = MAX_RECORD_AMOUNT;
        
        char buffer[buff_size];
        while ((fgets(buffer, sizeof(buffer), file) != NULL) && rc == STATUS_NO_ERROR && line_cnt < MAX_RECORD_AMOUNT)
        {
            buffer[strcspn(buffer, "\n")] = '\0';
            rc = read_one_record(buffer, &records[line_cnt]);
            line_cnt++;
        }
    }

    (*record_cnt) = line_cnt;

    return rc;
}

static void print_record(Record_t *records, size_t current_index)
{
    const int COL_WIDTH[13] = {14, 12, 9, 4, 5, 9, 13, 6, 9, 11, 6, 6, 11};

    char travel_time_buf[16];
    char leisure_cost_buf[16];
    char object_count_buf[16] = "";
    char air_temp_buf[16] = "";
    char water_temp_buf[16] = "";

    snprintf(travel_time_buf, sizeof travel_time_buf, "%.2f", records[current_index].travel_time);
    snprintf(leisure_cost_buf, sizeof leisure_cost_buf, "%.2f", records[current_index].leisure_cost);

    const char *tourism_kind_str   = "";
    const char *object_description = "";
    const char *season_description = "";
    const char *air_temp_str       = "";
    const char *water_temp_str     = "";
    const char *sport_type_str     = "";

    switch (records[current_index].kind) {
    case SIGHTSEEING:
        tourism_kind_str = "Экскурсионный";
        snprintf(object_count_buf, sizeof object_count_buf, "%d", records[current_index].tourism.sightseeing_obj.object_count);
        object_description = records[current_index].tourism.sightseeing_obj.main_obj;
        break;

    case BEACH:
        tourism_kind_str = "Пляжный";
        season_description = records[current_index].tourism.beach_obj.main_season;
        snprintf(air_temp_buf, sizeof air_temp_buf, "%d", records[current_index].tourism.beach_obj.air_temperature);
        snprintf(water_temp_buf, sizeof water_temp_buf, "%d", records[current_index].tourism.beach_obj.water_temperature);
        air_temp_str   = air_temp_buf;
        water_temp_str = water_temp_buf;
        break;

    case SPORT:
        tourism_kind_str = "Спортивный";
        sport_type_str = records[current_index].tourism.sport_obj.sport_type;
        break;
    }

    const char *cells[13] = 
    {
        records[current_index].country,
        records[current_index].capital,
        records[current_index].continent,
        records[current_index].visa,
        travel_time_buf,
        leisure_cost_buf,
        tourism_kind_str,
        object_count_buf,
        object_description,
        season_description,
        air_temp_str,
        water_temp_str,
        sport_type_str
    };


    for (int j = 0; j < 13; j++) 
    {
        printf("| ");
        pad(cells[j], COL_WIDTH[j]);
        printf(" ");
    }
    printf("|\n");
}

static void print_headers(void)
{
    const int COL_WIDTH[13] = {14, 12, 9, 4, 5, 9, 13, 6, 9, 11, 6, 6, 11};

    const char *HEADERS[13] = 
    {
        "Страна",  "Столица", "Материк", "Виза",
        "Время",   "Цена",    "Туризм",  "Кол-во",
        "Объекты", "Сезон",   "T возд",  "T воды", "Спорт"
    };

    for (int i = 0; i < 13; i++) 
    {
        printf("| ");
        pad(HEADERS[i], COL_WIDTH[i]);
        printf(" ");
    }
    printf("|\n"); 
}

void print_table(Record_t *records, size_t rec_len)
{
    print_headers();
    for (size_t i = 0; i < rec_len; i++) 
    {
        print_record(records, i);
    }
}

void print_requested_fields(Record_t *records, size_t rec_len, req_t *requests)
{
    print_headers();
    for (size_t j = 0; j < rec_len; j++)
    {
        if (records[j].kind == SPORT && strcmp(records[j].continent, requests->requested_continent) == 0 &&
             strcmp(records[j].tourism.sport_obj.sport_type, requests->requested_sport_type) == 0 &&
              records[j].leisure_cost <= requests->requested_price) {
                print_record(records, j);
            }
    }
}

void print_table_via_keys(Record_t *records, int *keys, size_t len)
{
    if (keys)
    {
        print_headers();
        for (size_t j = 0; j < len; j++)
        {
            print_record(records, (size_t)keys[j]);
        }
    }
}