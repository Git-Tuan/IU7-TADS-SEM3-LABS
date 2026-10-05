#include <stdio.h>
#include "table_core.h"
#include "table_io.h"
#include "file_utils.h"
#include <string.h>
#include <stdlib.h>


int main(void)
{
    rc_t rc = STATUS_NO_ERROR;

    Record_t records[MAX_RECORD_AMOUNT];
    size_t record_len = 0;
    char filename[MAX_FILENAME];
    
    rc = read_filename(filename);

    if (rc == STATUS_NO_ERROR)
    {
        filename[strcspn(filename, "\n")] = '\0';
        rc = read_records_from_file(filename, records, &record_len);
    }
    if (rc == STATUS_NO_ERROR)
    {
        rc = menu(records, &record_len);
    }
    if (rc != STATUS_NO_ERROR)
    {
        print_error_msg(rc);
    }

    return rc == STATUS_NO_ERROR ? EXIT_SUCCESS : EXIT_FAILURE;
}
