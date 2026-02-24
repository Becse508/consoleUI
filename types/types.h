#ifndef TYPES_HEADER_FOR_MY_SHIT
#define TYPES_HEADER_FOR_MY_SHIT

#include <windows.h>

typedef struct point {
    int x;
    int y;
} point;
typedef struct pointf {
    float x;
    float y;
} pointf;
typedef struct rect {
    int left;
    int top;
    int right;
    int bottom;
} rect;

#endif
