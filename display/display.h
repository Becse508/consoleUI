#ifndef DISPLAY_HEADER_FOR_MY_SHIT
#define DISPLAY_HEADER_FOR_MY_SHIT

#include "../collections/collections.h"
#include "../types/types.h"
#include "../surface/surface.h"
#include "../element/element.h"
#include "../drawing/drawing.h"


list *display_init(int sizex, int sizey);
void display_update();
bool display_draw();
void display_clear();

#endif
