#ifndef SURFACE_HEADER_FOR_MY_SHIT
#define SURFACE_HEADER_FOR_MY_SHIT

#include "../types/types.h"

typedef struct surface
{
    point size;
    CHAR_INFO texture[];
} surface;

size_t surface_get_index(point size, int x, int y);
CHAR_INFO surface_get_pixel(const surface *surf, int x, int y);
int surface_set_pixel(surface *surf, int x, int y, CHAR_INFO value);

void surface_init(surface *surf, point size);
surface* surface_create(point size);

void surface_fill(surface *surf, CHAR_INFO value);
int surface_paste(const surface *origin, surface *destination, point pos);
void surface_clear(surface *surf);

surface* string_to_surface_char(const char *str, int attributes);
surface* string_to_surface_wchar(const WCHAR *str, int attributes);

#define string_to_surface(str, attributes) \
    _Generic((str), \
    char: string_to_surface_char \
    WCHAR: string_to_surface_wchar \
)(str, attributes)

#endif
