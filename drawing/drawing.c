#include "drawing.h"
#include <math.h>

void draw_borders(surface *surf, border_chars characters) {
    if (surf->size.x * surf->size.y == 0) return;

    if (surf->size.x * surf->size.y == 1) {
        surf->texture[0] = (CHAR_INFO){'O', characters.horizontal.Attributes};
    }
    if (surf->size.x == 1) {
        surface_fill(surf, characters.horizontal);
    }
    else if (surf->size.y == 1) {
        surface_fill(surf, characters.vertical);
    }
    else {
        // CORNERS
        surf->texture[0] = characters.topleft;
        surface_set_pixel(surf, surf->size.x-1, 0, characters.topright);
        surface_set_pixel(surf, 0, surf->size.y-1, characters.bottomleft);
        surface_set_pixel(surf, surf->size.x-1, surf->size.y-1, characters.bottomright);

        // SIDES
        for (size_t i = 1; i < surf->size.x-1; i++)
        {
            surf->texture[i] = characters.horizontal;
            surface_set_pixel(surf, i, surf->size.y-1, characters.horizontal);
        }
        for (size_t i = 1; i < surf->size.y-1; i++)
        {
            surface_set_pixel(surf, 0, i, characters.vertical);
            surface_set_pixel(surf, surf->size.x-1, i, characters.vertical);
        }
    }
}
int draw_line(surface *surf,
               point start,
               point end,
               line_chars characters)
{
    CHAR_INFO horizontal = characters.horizontal_bottom;

    int error = 0;

    int x0, x1, y0, y1;
    if (start.x < end.x) {
        x0 = start.x;
        y0 = start.y;
        x1 = end.x;
        y1 = end.y;
    }
    else {
        x0 = end.x;
        y0 = end.y;
        x1 = start.x;
        y1 = start.y;
    }

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    // line at top (overline) if going downwards
    if (sy == -1) {
        horizontal = characters.horizontal_top;
    }

    int err = dx - dy;

    while (abs(x1 - x0) >= 1 || abs(y1 - y0) >= 1)
    {
        int old_x = x0;
        int old_y = y0;

        int e2 = err << 1;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }

        /* Determine movement type */
        int moved_x = (x0 != old_x);
        int moved_y = (y0 != old_y);

        CHAR_INFO pixel;

        if (moved_x && moved_y) {
            pixel = (sx == sy) ? characters.downtilt : characters.uptilt;
        }
        else if (moved_x) {
            pixel = horizontal;
        }
        else {
            pixel = characters.vertical;
        }
        
        error |= surface_set_pixel(surf, x0, y0, pixel);
    }

    return error;
}

void draw_line_braille(surface *surf, point start, point end, int attr, float density)
{
    // full braille pixel grid
    int pixel_width  = surf->size.x * 2;
    int pixel_height = surf->size.y * 4;

    // start/end in pixel coordinates
    float x0 = start.x * 2.0f;
    float y0 = start.y * 4.0f;
    float x1 = end.x   * 2.0f;
    float y1 = end.y   * 4.0f;

    // vector from start -> end
    float dx = x1 - x0;
    float dy = y1 - y0;

    // number of steps based on density and longest axis
    float length = fmaxf(fabsf(dx), fabsf(dy));
    if (length == 0.0f) length = 1.0f; // avoid division by zero

    int steps = (int)(length * density);

    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        int px = (int)(x0 + dx * t + 0.5f);
        int py = (int)(y0 + dy * t + 0.5f);

        // clip to surface bounds
        if (px < 0 || px >= pixel_width || py < 0 || py >= pixel_height)
            continue;

        int cell_x = px / 2;
        int cell_y = py / 4;
        int sub_x  = px % 2;
        int sub_y  = py % 4;

        int bit;
        switch(sub_y) {
            case 0: bit = (sub_x == 0) ? 0 : 3; break;
            case 1: bit = (sub_x == 0) ? 1 : 4; break;
            case 2: bit = (sub_x == 0) ? 2 : 5; break;
            case 3: bit = (sub_x == 0) ? 6 : 7; break;
        }

        CHAR_INFO cell = surface_get_pixel(surf, cell_x, cell_y);
        if (cell.Char.UnicodeChar < 0x2800 || cell.Char.UnicodeChar > 0x28FF)
            cell.Char.UnicodeChar = 0x2800;
        cell.Char.UnicodeChar |= 1 << bit;
        cell.Attributes = attr;
        surface_set_pixel(surf, cell_x, cell_y, cell);
    }
}


int border_is_empty(const border_chars *b) {
    return b->horizontal.Char.UnicodeChar == 0 && b->horizontal.Attributes == 0
        && b->vertical.Char.UnicodeChar == 0 && b->vertical.Attributes == 0
        && b->topleft.Char.UnicodeChar == 0 && b->topleft.Attributes == 0
        && b->topright.Char.UnicodeChar == 0 && b->topright.Attributes == 0
        && b->bottomleft.Char.UnicodeChar == 0 && b->bottomleft.Attributes == 0
        && b->bottomright.Char.UnicodeChar == 0 && b->bottomright.Attributes == 0;
}
