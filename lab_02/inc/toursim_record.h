#ifndef TOURISM_RECORD
#define TOURISM_RECORD
#include "defines.h"

typedef enum { 
    SIGHTSEEING,
    BEACH,
    SPORT
} tourism_t;

struct sightseeing
{
    int object_count; 
    char main_obj[MAX_OBJECT_NAME];
};

struct beach
{ 
    char main_season[MAX_MAIN_SEASON_NAME]; 
    int air_temperature; 
    int water_temperature;
};

struct sport
{
    char sport_type[MAX_SPORT_TYPE_NAME];
};


typedef struct 
{
    char country[MAX_COUNTRY_NAME];
    char capital[MAX_CAPITAL_NAME];
    char continent[MAX_CONTINENT_NAME];
    char visa[MAX_VISA_NAME];
    double travel_time;
    double leisure_cost;
    tourism_t kind;
    union tourism_type
    {
        struct sightseeing sightseeing_obj;
        struct beach beach_obj;
        struct sport sport_obj;
    } tourism;

} Record_t;

#endif
