#include "element.h"

void element_init(element *elem, unsigned int flags) {
    elem->selected_surface = 0;
    elem->pos = (point){0};
    elem->flags = flags;
    elem->style = (element_style){0};
    pvector_surface_ptr_init(&elem->surfaces);
}

int has_style_changed(element *elem) {
    if (elem->style.surf == 0) return 1;
    return memcmp(&elem->style, &elem->__old_style, sizeof(element_style) - sizeof(surface*)); // ignore surface pointer
}

void element_update_style(element *elem) {
    if (elem->flags & ELEMENT_DONT_USE_STYLE) return;

    if(elem->style.surf == 0 ||
       elem->style.surf->size.x != active_surface_ptr(elem)->size.x ||
       elem->style.surf->size.y != active_surface_ptr(elem)->size.y)
    {
        free(elem->style.surf);
        elem->style.surf = surface_create(active_surface_ptr(elem)->size);
    }
    else if (has_style_changed(elem) == 0) // only return if neither changed
        return;

    elem->__old_style = elem->style;

    if (elem->style.fill_char.Char.UnicodeChar) {
        surface_fill(elem->style.surf, elem->style.fill_char);
    }
    if (!border_is_empty(&elem->style.border)) {
        draw_borders(elem->style.surf, elem->style.border);
    }
}

element* element_create(unsigned int flags) {
    element *elem = malloc(sizeof(element));
    element_init(elem, flags);
    return elem;
}

/// @brief Frees all surfaces the element contains
/// @param elem 
void element_destroy(element *elem) {
    pvector_surface_ptr_destroy(&elem->surfaces);
    free(elem->style.surf);
    elem->style.surf = 0;
}

void element_add_surface(element *elem, surface *surf) {
    pvector_surface_ptr_push(&elem->surfaces, surf);
}

/// @brief Creates `count` number of empty surfaces an adds them to the end of the `surfaces` vector
/// @param size size of the surfaces
/// @param count number of surfaces to add
void element_make_surfaces(element *elem, point size, size_t count) {
    for (size_t i = 0; i < count; i++)
        element_add_surface(elem, surface_create(size));
}
