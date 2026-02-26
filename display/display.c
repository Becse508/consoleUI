#include "display.h"
#include <stdlib.h>
#include <stdio.h>
#include "timer.h"

static CHAR_INFO *buffer = 0;
list elements = {0};
int sizex;
int sizey;
timer_t timer;

/// @return Internal element list. Use it to append elements.
list *display_init(int size_x, int size_y) {
    if (buffer != 0) free(buffer);

    sizex = size_x;
    sizey = size_y;
    buffer = malloc(sizeof(CHAR_INFO) * sizex * sizey);
    return &elements;
    timer_init();
    timer_start(&timer);
}

size_t get_buffer_index(int x, int y) {
    if (x < 0 || y < 0 || x >= sizex || y >= sizey) return SIZE_MAX;
    return y * sizex + x;
}

// no idea if this makes ts faster
void place_surface(surface *surf, point pos)
{
    if (!surf) return;

    int start_x = pos.x;
    int start_y = pos.y;

    int end_x = start_x + surf->size.x;
    int end_y = start_y + surf->size.y;

    if (end_x <= 0 || end_y <= 0 || start_x >= sizex || start_y >= sizey)
        return;

    int src_x0 = start_x < 0 ? -start_x : 0;
    int src_y0 = start_y < 0 ? -start_y : 0;

    int src_x1 = end_x > sizex ? sizex - start_x : surf->size.x;
    int src_y1 = end_y > sizey ? sizey - start_y : surf->size.y;

    for (int y = src_y0; y < src_y1; y++)
    {
        CHAR_INFO *src_row = &surf->texture[y * surf->size.x];
        CHAR_INFO *dst_row = &buffer[(start_y + y) * sizex];

        for (int x = src_x0; x < src_x1; x++)
        {
            CHAR_INFO pixel = src_row[x];
            if (pixel.Char.UnicodeChar != 0)
                dst_row[start_x + x] = pixel;
        }
    }
}

void place_element(element *elem) {
    if (!(elem->flags & ELEMENT_DONT_USE_STYLE)) {
        element_update_style(elem);
        place_surface(elem->style.surf, elem->pos);
    }
    
    if (elem->flags & ELEMENT_RENDER_ALL_SURFACES) {
        // funny loop
        for (size_t i = elem->surfaces.count-1; i < SIZE_MAX; i--)
            place_surface(element_get_surface_ptr(elem, i), elem->pos);
    } 
    else {
        place_surface(active_surface_ptr(elem), elem->pos);
    }
}

double display_get_delta_time() {
    double delta_time = timer_elapsed(&timer);
    timer_start(&timer);
    return delta_time;
}

void display_update() {
    list_node *node = elements.head;
    while (node)
    {
        place_element(node->value);
        node = node->next;
    }
}

bool display_draw() {
    SMALL_RECT rect = {0, 0, sizex - 1, sizey - 1};
    return WriteConsoleOutputW(GetStdHandle(STD_OUTPUT_HANDLE), buffer, (COORD){sizex, sizey}, (COORD){0}, &rect);
}


void display_clear() {
    memset(buffer, 0, sizeof(CHAR_INFO) * sizex * sizey);
}
