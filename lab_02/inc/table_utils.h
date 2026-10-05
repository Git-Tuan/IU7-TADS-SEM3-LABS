#ifndef TABLE_UTILS_H
#define TABLE_UTILS_H

#include "errors.h"
#include "toursim_record.h"
#include <stddef.h>

rc_t add_record(Record_t *records, size_t *rec_len);
rc_t delete_records_by_country(Record_t *records, size_t *rec_len);
rc_t process_continent_field(Record_t *records, size_t rec_len);

#endif