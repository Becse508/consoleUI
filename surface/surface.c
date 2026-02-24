#include "surface.h"
#include <stdlib.h>

#define check_out_of_bounds(surf, x, y) \
    ((x) < 0 || (y) < 0 ||              \
     (size_t)(x) >= (surf)->size.x ||   \
     (size_t)(y) >= (surf)->size.y)


void surface_init(surface *surf, point size) {
    surf->size = size;
    memset(surf->texture, 0, sizeof(CHAR_INFO) * size.x * size.y);
}
surface* surface_create(point size) {
    surface *surf = malloc(sizeof(surface) + sizeof(CHAR_INFO) * size.x * size.y);
    surface_init(surf, size);
    return surf;
}

size_t surface_get_index(point size, int x, int y) {
    return y * size.x + x;
}
CHAR_INFO surface_get_pixel(const surface *surf, int x, int y) {
    return surf->texture[surface_get_index(surf->size, x, y)];
}
int surface_set_pixel(surface *surf, int x, int y, CHAR_INFO value) {
    if (check_out_of_bounds(surf, x, y))
        return 1;

    surf->texture[surface_get_index(surf->size, x, y)] = value;
    return 0;
}

void surface_fill(surface *surf, CHAR_INFO value) {
    if (!surf) return;
    for (size_t i = 0; i < surf->size.x * surf->size.y; i++)
    {
        surf->texture[i] = value;
    }
}
void surface_clear(surface *surf) {
    memset(surf->texture, 0, sizeof(CHAR_INFO) * surf->size.x * surf->size.y);
}

/// @brief Pastes origin onto destination
/// @param pos top left starting point
/// @return 1 if any pixel was out of bounds, 0 otherwise
int surface_paste(const surface *origin, surface *destination, point pos) {
    int error = 0;
    for (size_t y = 0; y < origin->size.y; y++) {
        for (size_t x = 0; x < origin->size.x; x++)
        {
            error |= surface_set_pixel(destination, pos.x + x, pos.y + y, surface_get_pixel(origin, x, y));
        }
    }
    return error;
}


#define STRTOSURF_INSIDE(T) \
const T *str_original = str; \
point size = {0, 1}; \
size_t current_width = 0; \
\
while (*str) \
{ \
    if (*str == '\n') { \
        size.y++; \
        if (current_width > size.x) \
            size.x = current_width; \
        current_width = 0; \
    } \
    else if (*str != '\r') { \
        current_width++; \
    } \
    str++; \
} \
if (current_width > size.x) \
    size.x = current_width; \
\
surface *surf = surface_create(size); \
\
size_t x = 0, y = 0; \
while (*str_original) \
{ \
    if (*str_original == '\n') { \
        x = 0; \
        y++; \
    } \
    else if (*str_original != '\r') { \
        surface_set_pixel(surf, x++, y, (CHAR_INFO){*str_original, attributes}); \
    } \
    str_original++; \
} \
    \
return surf;

surface* string_to_surface_wchar(const WCHAR *str, int attributes) {
    STRTOSURF_INSIDE(WCHAR)
}
surface* string_to_surface_char(const char *str, int attributes) {
    STRTOSURF_INSIDE(char)
}
