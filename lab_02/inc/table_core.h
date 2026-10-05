#ifndef TABLE_CORE_H
#define TABLE_CORE_H
#include "errors.h"
#include "defines.h"
#include "toursim_record.h"

typedef enum {
    MENU_EXIT                   = 0,
    MENU_ADD                    = 1,
    MENU_DELETE_BY_NAME         = 2,
    MENU_PRINT_TABLE            = 3,
    MENU_SELECTION_SORT_TABLE   = 4,
    MENU_QUICK_SORT_TABLE       = 5,
    MENU_KEYS_SELECTION_SORT    = 6,
    MENU_KEYS_QUICK_SORT        = 7,
    MENU_PRINT_KEYS             = 8,
    MENU_FILTER_BY_CONT         = 9,
    MENU_MEASURE_SORT_TIME      = 10,
    MENU_PRINT_TABLE_VIA_KEYS   = 11,
    MENU_SAVE_TABLE_TO_FILE     = 12
} menu_item_t;


rc_t menu(Record_t *records, size_t *rec_len);

#endif
