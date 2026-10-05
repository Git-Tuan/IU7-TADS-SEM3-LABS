#ifndef TABLE_IO_H
#define TABLE_IO_H
#include "errors.h"
#include "toursim_record.h"

#include <stddef.h>

typedef struct req_t
{
    char requested_continent[MAX_CONTINENT_NAME]; 
    char requested_sport_type[MAX_SPORT_TYPE_NAME];
    double requested_price;
} req_t;

void print_table(Record_t *records, size_t size);
void print_requested_fields(Record_t *records, size_t rec_len, req_t *requests);
void print_table_via_keys(Record_t *records, int *keys, size_t len);
rc_t read_filename(char *filename);
rc_t read_records_from_file(const char *filename, Record_t *records, size_t *record_cnt);



#endif
