#include "file_utils.h"
#include <stdio.h>
#include <string.h>

rc_t save_table(Record_t *records, size_t size)
{
    rc_t rc = STATUS_NO_ERROR;
    FILE *file = NULL;
    char filename[MAX_FILENAME];

    if (records == NULL)
        rc = STATUS_NULL_POINTER_ERR;

    if (rc == STATUS_NO_ERROR)
    {
        printf("Введите путь к файлу для сохранения: ");
        if (fgets(filename, MAX_FILENAME, stdin) == NULL)
            rc = STATUS_STRING_INPUT_ERROR;
    }

    if (rc == STATUS_NO_ERROR)
    {
        filename[strcspn(filename, "\n")] = '\0';
        file = fopen(filename, "w");
        if (file == NULL)
            rc = STATUS_FILE_OPEN_ERROR;
    }

    if (rc == STATUS_NO_ERROR)
    {
        for (size_t idx = 0; idx < size && rc == STATUS_NO_ERROR; idx++)
        {
            if (fprintf(file, "%s|%s|%s|%s|%lf|%lf|",
                        records[idx].country,
                        records[idx].capital,
                        records[idx].continent,
                        records[idx].visa,
                        records[idx].travel_time,
                        records[idx].leisure_cost) < 0)
            {
                rc = STATUS_FILE_WRITE_ERROR;
                break;
            }

            switch (records[idx].kind)
            {
            case SIGHTSEEING:
                if (fprintf(file, "Экскурсионный|%d|%s",
                            records[idx].tourism.sightseeing_obj.object_count,
                            records[idx].tourism.sightseeing_obj.main_obj) < 0)
                    rc = STATUS_FILE_WRITE_ERROR;
                break;

            case BEACH:
                if (fprintf(file, "Пляжный|%s|%d|%d",
                            records[idx].tourism.beach_obj.main_season,
                            records[idx].tourism.beach_obj.air_temperature,
                            records[idx].tourism.beach_obj.water_temperature) < 0)
                    rc = STATUS_FILE_WRITE_ERROR;
                break;

            case SPORT:
                if (fprintf(file, "Спортивный|%s",
                            records[idx].tourism.sport_obj.sport_type) < 0)
                    rc = STATUS_FILE_WRITE_ERROR;
                break;
            }

            fputc('\n', file);
        }
    }

    if (file != NULL)
        fclose(file);

    return rc;
}
