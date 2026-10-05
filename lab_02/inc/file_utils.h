#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "errors.h"
#include "toursim_record.h"
#include "stddef.h"

rc_t save_table(Record_t *records, size_t size);

#endif
