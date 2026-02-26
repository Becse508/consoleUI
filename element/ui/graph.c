#include "graph.h"
#include "../../drawing/drawing.h"
#include <math.h>
#include <stdio.h>

void graph_data_init(graph_data *v, unsigned char flags) { 
    v->count = 0; 
    v->capacity = 10; 
    v->data = malloc(sizeof(pointf) * 10); 
    v->flags = flags;
} 
void graph_data_init_minmax(graph_data *v, float min, float max, unsigned char flags) {
    graph_data_init(v, flags);
    v->min = min;
    v->max = max;
}

void check_set_minmax(graph_data *v, float value) {
    if (value < v->min) v->min = value;
    if (value > v->max) v->max = value;
}
int check_minmax(const graph_data *v, float value) {
    if (value < v->min) return 0;
    if (value > v->max) return 0;
    return 1;
}

void graph_data_push(graph_data *v, pointf value) {
    if (v->flags & GRAPH_AUTO_MINMAX) {
        if (v->count == 0) {
            v->min = value.y;
            v->max = value.y;
        } else {
            if (value.y < v->min) v->min = value.y;
            if (value.y > v->max) v->max = value.y;
        }
    }
    
    if (v->count == v->capacity) { 
        v->capacity *= 2; 
        v->data = realloc(v->data, sizeof(pointf) * v->capacity); 
    }

    size_t i = 0;
    while (i < v->count && v->data[i].x < value.x)
        i++;
    
    for (size_t j = v->count; j > i; j--)
        v->data[j] = v->data[j-1];
    
    v->data[i] = value;
    v->count++;
} 

void graph_data_update_minmax(graph_data *v) {
    if (v->flags & GRAPH_AUTO_MINMAX) {
        if (v->count == 0) {
            v->min = 0; v->max = 0;
            return;
        }
    
        v->min = v->data[0].y;
        v->max = v->data[0].y;
        for (size_t i = 0; i < v->count; i++)
            check_set_minmax(v, v->data[i].y);
    }
}

void graph_data_remove(graph_data *v, size_t index) { 
    if (index >= v->count) return; 
    
    if (index == v->count-1) { 
        v->count--; 
        return; 
    } 
    
    for (size_t i = index; i < v->count-1; i++) 
    { 
        v->data[i] = v->data[i+1]; 
    } 
    v->count--;
    graph_data_update_minmax(v); 
} 

void graph_data_destroy(graph_data *v) { 
    free(v->data); 
    v->data = 0; 
    v->count = 0; 
    v->capacity = 0; 
} 

void graph_data_clear(graph_data *v) { 
    v->count = 0; 
}

size_t graph_data_find_index(graph_data *v, int (*matchfn)(pointf)) { 
    for (int i = 0; i < v->count; i++) { 
        if (matchfn(v->data[i])) 
            return i; 
    } 
    return SIZE_MAX; 
}
void graph_data_leftshift_push(graph_data *v, pointf value) {
    graph_data_push(v, value);
    graph_data_remove(v, 0);
}


point calc_node_pos(rect area, pointf data, float minx, float miny, float dx, float dy) {
    int sizex = area.right - area.left + 1;
    int sizey = area.bottom - area.top + 1;

    int x = area.left + (data.x - minx) / dx * sizex;
    int y = (data.y - miny) / dy * sizey;
    // flip y for top-down y
    y = area.top + (sizey - y); 

    return (point){x, y};
}

/// @brief Renders the graph data onto the surface
/// @param area All bounds are inclusive. For a surface with size (x, y), use (0, 0, x-2, y-2) for full cover.
/// @param flags macros GRAPH_RENDER_* or GRAPH_DONT_RENDER_*
void ui_graph_render(graph_data *grap, surface *surf, rect area, line_chars characters, unsigned char flags) {
    if (grap->count == 0)
        return;

    float minx = grap->data[0].x;
    float maxx = grap->data[grap->count-1].x;
    float dy = grap->max - grap->min;
    float dx = maxx - minx;
    if (dx == 0) dx = 1;
    if (dy == 0) dy = 1;

    point prev_pos = calc_node_pos(area, grap->data[0], minx, grap->min, dx, dy);
    if (flags & GRAPH_RENDER_NODES) {
        surface_set_pixel(surf, prev_pos.x, prev_pos.y, (CHAR_INFO){L'█', characters.horizontal_bottom.Attributes});
    }

    for (size_t i = 1; i < grap->count; i++)
    {
        point current_pos = calc_node_pos(area, grap->data[i], minx, grap->min, dx, dy);

        if (!(flags & GRAPH_DONT_RENDER_LINES)) {
            draw_line(surf, prev_pos, current_pos, characters);
        }
        if (flags & GRAPH_RENDER_NODES) {
            surface_set_pixel(surf, current_pos.x, current_pos.y, (CHAR_INFO){L'█', characters.horizontal_bottom.Attributes});
        }

        prev_pos = current_pos;
    }
}

/// @brief Renders the graph data onto the surface, using braille characters
/// @param area All bounds are inclusive. For a surface with size (x, y), use (0, 0, x-2, y-2) for full cover.
/// @param flags macros GRAPH_RENDER_* or GRAPH_DONT_RENDER_*
void ui_graph_render_braille(const graph_data *grap, surface *surf, rect area, int attr, float density, unsigned char flags) {
    if (grap->count == 0)
        return;

    float minx = grap->data[0].x;
    float maxx = grap->data[grap->count-1].x;
    float dy = grap->max - grap->min;
    float dx = maxx - minx;
    if (dx == 0) dx = 1;
    if (dy == 0) dy = 1;

    point prev_pos = calc_node_pos(area, grap->data[0], minx, grap->min, dx, dy);
    if (flags & GRAPH_RENDER_NODES) {
        surface_set_pixel(surf, prev_pos.x, prev_pos.y, (CHAR_INFO){L'█', attr});
    }

    for (size_t i = 1; i < grap->count; i++)
    {
        point current_pos = calc_node_pos(area, grap->data[i], minx, grap->min, dx, dy);

        if (!(flags & GRAPH_DONT_RENDER_LINES)) {
            draw_line_braille(surf, prev_pos, current_pos, attr, density);
        }
        if (flags & GRAPH_RENDER_NODES) {
            surface_set_pixel(surf, current_pos.x, current_pos.y, (CHAR_INFO){L'█', attr});
        }

        prev_pos = current_pos;
    }
}

/// @brief Renders value indicators onto the surface
/// @param count How many indicators there should be in each diraction
void ui_graph_render_indicators(const graph_data *grap, surface *surf, point count, int attributes) {
    if (!grap || !surf) return;

    float minx = grap->data[0].x;
    float maxx = grap->data[grap->count-1].x;
    float dy = grap->max - grap->min;
    float dx = minx - maxx;

    char buffer[6];

    for (size_t i = 0; i < count.x; i++) {
        int pos = (i / (float)(count.x - 1)) * (surf->size.x - 1);
        float value = (i / (float)(count.x - 1)) * dx;

        snprintf(buffer, sizeof(buffer), "%.2f", value);
        surface *indicator = string_to_surface_char(buffer, attributes);
        surface_paste(indicator, surf, (point){pos, 0});
        free(indicator);
    }
    for (size_t i = 0; i < count.y; i++) {
        int pos = surf->size.y - (i / (float)(count.y - 1)) * (surf->size.y - 1) - 1;
        float value = (i / (float)(count.y - 1)) * dy;

        snprintf(buffer, sizeof(buffer), "%.2f", value);
        surface *indicator = string_to_surface_char(buffer, attributes);
        surface_paste(indicator, surf, (point){0, pos});
        free(indicator);
    }
}

/// @brief Renders a nice grid for the graph on the surface
/// @param count how many in each direction
/// @param characters only the `horizontal_bottom` and/or `vertical` values are used
/// @param flags macros GRAPH_GRID_RENDER_*
void ui_graph_render_grid(surface *surf, point count, line_chars characters, unsigned char flags) {
    if (!surf) return;

    if (flags & GRAPH_GRID_RENDER_VERTICAL) {
        for (size_t i = 0; i < count.x; i++) {
            int pos = (i / (float)(count.x - 1)) * (surf->size.x - 1);
            for (size_t y = 0; y < surf->size.y; y++)
                surface_set_pixel(surf, pos, y, characters.vertical);
        }
    }
    if (flags & GRAPH_GRID_RENDER_HORIZONTAL) {
        for (size_t i = 0; i < count.y; i++) {
            int pos = (i / (float)(count.y - 1)) * (surf->size.y - 1);
            for (size_t x = 0; x < surf->size.x; x++)
                surface_set_pixel(surf, x, pos, characters.horizontal_bottom);
        }
    }
}
