#ifndef MY_DRAWING_HEADER_TYPE_SHIT
#define MY_DRAWING_HEADER_TYPE_SHIT

#include "../surface/surface.h"


typedef struct {
    CHAR_INFO horizontal;
    CHAR_INFO vertical;
    CHAR_INFO topleft;
    CHAR_INFO topright;
    CHAR_INFO bottomleft;
    CHAR_INFO bottomright;
} border_chars;
typedef struct {
    CHAR_INFO horizontal_bottom;
    CHAR_INFO horizontal_top;
    CHAR_INFO vertical;
    CHAR_INFO uptilt;
    CHAR_INFO downtilt;
} line_chars;

int border_is_empty(const border_chars *b);


#define BORDER_CHARS_SINGLE(attributes) (border_chars){ \
    (CHAR_INFO){L'─', attributes}, \
    (CHAR_INFO){L'│', attributes}, \
    (CHAR_INFO){L'┌', attributes}, \
    (CHAR_INFO){L'┐', attributes}, \
    (CHAR_INFO){L'└', attributes}, \
    (CHAR_INFO){L'┘', attributes}  \
}
#define BORDER_CHARS_BOLD(attributes) (border_chars){ \
    (CHAR_INFO){L'━', attributes}, \
    (CHAR_INFO){L'┃', attributes}, \
    (CHAR_INFO){L'┏', attributes}, \
    (CHAR_INFO){L'┓', attributes}, \
    (CHAR_INFO){L'┗', attributes}, \
    (CHAR_INFO){L'┛', attributes}  \
}
#define LINE_CHARS_DEFAULT(attributes) (line_chars){ \
    (CHAR_INFO){L'_', attributes}, \
    (CHAR_INFO){L'‾', attributes}, \
    (CHAR_INFO){L'|', attributes}, \
    (CHAR_INFO){L'/', attributes}, \
    (CHAR_INFO){L'\\', attributes} \
}
#define LINE_CHARS_SMOOTH(attributes) (line_chars){ \
    (CHAR_INFO){L'_', attributes}, \
    (CHAR_INFO){L'‾', attributes}, \
    (CHAR_INFO){L'│', attributes}, \
    (CHAR_INFO){L'╱', attributes}, \
    (CHAR_INFO){L'╲', attributes}  \
}
#define LINE_CHARS_SMOOTH_NOCONNECT(attributes) (line_chars){ \
    (CHAR_INFO){L'─', attributes}, \
    (CHAR_INFO){L'─', attributes}, \
    (CHAR_INFO){L'│', attributes}, \
    (CHAR_INFO){L'╱', attributes}, \
    (CHAR_INFO){L'╲', attributes}  \
}


void draw_borders(surface *surf, border_chars characters);

/// @brief Draw a line from the characters provided.
/// @param start exclusive
/// @param end exclusive
/// @param characters use `LINE_CHARS_DEFAULT` for a normal line
/// @return 0 on success, 1 if any index was out of bounds
int draw_line(surface *surf,
              point start,
              point end,
              line_chars characters);

/// @brief (written by ChatGPT) Draws a line between `start` and `end` using braille characters for precision
/// @param attr CHAR_INFO attributes
void draw_line_braille(surface *surf, point start, point end, int attr, float density);

#endif
