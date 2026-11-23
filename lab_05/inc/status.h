#ifndef STATUS_H
#define STATUS_H

typedef enum
{
    STATUS_OK = 0,
    STATUS_POP_ERR = 1,
    STATUS_MEM_ERR = 2,
    STATUS_PUSH_ERR = 3,
    STATUS_INP_ERR = 4,
    STATUS_PARAM_ERR = 5
} rc_t;

#endif
