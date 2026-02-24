#pragma once

#include "../../element/element.h"
#include "../../collections/vector.h"
#include "../../drawing/drawing.h"

#define GRAPH_RENDER_NODES (1u << 0)
#define GRAPH_DONT_RENDER_LINES (1u << 1)
#define GRAPH_GRID_RENDER_VERTICAL (1u << 0)
#define GRAPH_GRID_RENDER_HORIZONTAL (1u << 1)

typedef struct graph_data 
{ 
    pointf *data;
    size_t capacity; 
    size_t count;
    float min;
    float max;
    unsigned char flags;
} graph_data; 

#define GRAPH_AUTO_MINMAX (1u << 0)


void graph_data_init(graph_data *v, unsigned char flags);
void graph_data_init_minmax(graph_data *v, float min, float max, unsigned char flags);
void graph_data_push(graph_data *v, pointf value);
void graph_data_leftshift_push(graph_data *v, pointf value);
void graph_data_remove(graph_data *v, size_t index);
void graph_data_destroy(graph_data *v);
void graph_data_clear(graph_data *v);
size_t graph_data_find_index(graph_data *v, int (*matchfn)(pointf));
void graph_data_update_minmax(graph_data *v);


void ui_graph_render(graph_data *grap, surface *surf, rect area, line_chars characters, unsigned char flags);
void ui_graph_render_braille(const graph_data *grap, surface *surf, rect area, int attr, float density, unsigned char flags);
void ui_graph_render_indicators(const graph_data *grap, surface *surf, point count, int attributes);
void ui_graph_render_grid(surface *surf, point count, line_chars characters, unsigned char flags);
