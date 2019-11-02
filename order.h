#include <time.h>

#ifndef _ORDER
#define _ORDER
typedef struct order
{
    time_t time;
    char name[40];
    char email[40];
    char phone[15];
    int efficiency;
} ORDER;
#endif