#include "display.h"
#include <stdlib.h>
#include <stdio.h>

static CHAR_INFO *buffer = 0;
list elements = {0};
int sizex;
int sizey;

/// @return Internal element list. Use it to append elements.
list *display_init(int size_x, int size_y) {
    if (buffer != 0) free(buffer);

    sizex = size_x;
    sizey = size_y;
    buffer = malloc(sizeof(CHAR_INFO) * sizex * sizey);
    return &elements;
}

size_t get_buffer_index(int x, int y) {
    if (x < 0 || y < 0 || x >= sizex || y >= sizey) return SIZE_MAX;
    return y * sizex + x;
}


void place_surface(surface *surf, point pos) {
    if (!surf) return;

    for (size_t y = 0; y < surf->size.y; y++)
    {
        for (size_t x = 0; x < surf->size.x; x++)
        {
            size_t idx = get_buffer_index(pos.x + x, pos.y + y);
            if (idx == SIZE_MAX) continue;

            CHAR_INFO pixel = surface_get_pixel(surf, x, y);
            if (pixel.Char.UnicodeChar != 0) {
                buffer[idx] = pixel;
            }
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
