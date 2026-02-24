#ifndef ELEMENT_FOR_MY_SHIT_HEADER_SHIT_TYPE_SHIT
#define ELEMENT_FOR_MY_SHIT_HEADER_SHIT_TYPE_SHIT

#include "../types/types.h"
#include "../surface/surface.h"
#include "../collections/collections.h"
#include "../drawing/drawing.h"

typedef surface* surface_ptr;
MAKE_PTRVECTOR(surface_ptr)

#define ELEMENT_RENDER_ALL_SURFACES (1u << 0)
#define ELEMENT_DONT_USE_STYLE      (1u << 1)
#define ELEMENT_SELECTABLE          (1u << 2)

typedef struct element_style {
    border_chars border;
    CHAR_INFO fill_char;
    surface *surf;
} element_style;

/// `__old_style` is internal. Used only for tracking modifications
typedef struct element
{
    point pos;
    element_style style;
    int selected_surface;
    pvector_surface_ptr surfaces;
    unsigned int flags;

    element_style __old_style;
} element;


#define active_surface_ptr(e) ((e)->surfaces.data[(e)->selected_surface])
#define active_surface(e) ((e).surfaces.data[(e).selected_surface])
#define element_get_surface(e, idx) ((e).surfaces.data[(idx)])
#define element_get_surface_ptr(e, idx) ((e)->surfaces.data[(idx)])


void element_init(element *elem, unsigned int flags);
element* element_create(unsigned int flags);
void element_destroy(element *elem);
void element_add_surface(element *elem, surface *surf);
void element_make_surfaces(element *elem, point size, size_t count);

void element_update_style(element *elem);

#endif
