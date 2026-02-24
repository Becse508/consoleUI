#include <stdio.h>
#include "display/display.h"
#include "element/ui.h"
#include <time.h>
#include <Windows.h>

#define WAITTIME 20
#define MAXVALS 100


static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
   static unsigned long long _previousTotalTicks = 0;
   static unsigned long long _previousIdleTicks = 0;

   unsigned long long totalTicksSinceLastTime = totalTicks-_previousTotalTicks;
   unsigned long long idleTicksSinceLastTime  = idleTicks-_previousIdleTicks;

   float ret = 1.0f-((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime)/totalTicksSinceLastTime : 0);

   _previousTotalTicks = totalTicks;
   _previousIdleTicks  = idleTicks;
   return ret;
}

static unsigned long long FileTimeToInt64(const FILETIME *ft) {return (((unsigned long long)(ft->dwHighDateTime))<<32)|((unsigned long long)ft->dwLowDateTime);}

// Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
// You'll need to call this at regular intervals, since it measures the load between
// the previous call and the current one.  Returns -1.0 on error.
float GetCPULoad()
{
   FILETIME idleTime, kernelTime, userTime;
   return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(&idleTime), FileTimeToInt64(&kernelTime)+FileTimeToInt64(&userTime)) : -1.0f;
}


void set_console_settings(int buffx, int buffy, int fsizex, int fsizey) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD buffer_size = {buffx, buffy};
    SetConsoleScreenBufferSize(hConsole, buffer_size);
    
    CONSOLE_FONT_INFOEX fi;
    fi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    GetCurrentConsoleFontEx(hConsole, 1, &fi);

    fi.dwFontSize = (COORD){fsizex, fsizey};
    SetCurrentConsoleFontEx(hConsole, 1, &fi);
}

graph_data cpugrap;
element *cpugraph_elem;

graph_data ramgrap;
element *ramgraph_elem;

list *display_elemlist;
struct timeb start_graph;
struct timeb current_time;
int x = 0;

void add_entry(graph_data *grap, element *graph_elem, pointf data, int clear) {
    if (grap->count <= MAXVALS) {
        graph_data_push(grap, data);
    } else {
        graph_data_leftshift_push(grap, data);
    }
    
    if (clear) {
        surface_fill(element_get_surface_ptr(graph_elem, 1), (CHAR_INFO){0});
    }
    ui_graph_render_braille(grap, element_get_surface_ptr(graph_elem, 1), (rect){1, 1, 129, 58}, 
        (grap->data[grap->count-1].y >= 80)
        ? 0x0004
            : ((grap->data[grap->count-1].y >= 50)
            ? 0x0006
            : 0x000a), 5, 0);
}



int main() {
    ftime(&start_graph);

    display_elemlist = display_init(500, 200);
    set_console_settings(500, 200, 5, 10);

    element *cputitle = element_create(0);
    element_add_surface(cputitle, string_to_surface_char("CPU usage", 0x000d));

    element *ramtitle = element_create(0);
    ramtitle->pos = (point){136, 0};
    element_add_surface(ramtitle, string_to_surface_char("RAM usage", 0x000b));

    cpugraph_elem = element_create(ELEMENT_RENDER_ALL_SURFACES | ELEMENT_DONT_USE_STYLE);
    cpugraph_elem->pos = (point){1, 1};
    graph_data_init_minmax(&cpugrap, 0, 100, 0);
    element_make_surfaces(cpugraph_elem, (point){132, 60}, 3);

    ramgraph_elem = element_create(ELEMENT_RENDER_ALL_SURFACES | ELEMENT_DONT_USE_STYLE);
    ramgraph_elem->pos = (point){136, 1};
    graph_data_init_minmax(&ramgrap, 0, 100, 0);
    element_make_surfaces(ramgraph_elem, (point){132, 60}, 3);
    
    element *fps_elem = element_create(0);
    fps_elem->pos = (point){250, 0};
    element_add_surface(fps_elem, surface_create((point){16, 5}));
    fps_elem->style.border = BORDER_CHARS_BOLD(0x000e);

    list_append_many(display_elemlist, 5, cpugraph_elem, ramgraph_elem, cputitle, ramtitle, fps_elem);

    ui_graph_render_grid(element_get_surface_ptr(cpugraph_elem, 2), (point){6,6}, LINE_CHARS_SMOOTH_NOCONNECT(0x0008), GRAPH_GRID_RENDER_HORIZONTAL | GRAPH_GRID_RENDER_VERTICAL);
    ui_graph_render_grid(element_get_surface_ptr(ramgraph_elem, 2), (point){6,6}, LINE_CHARS_SMOOTH_NOCONNECT(0x0008), GRAPH_GRID_RENDER_HORIZONTAL | GRAPH_GRID_RENDER_VERTICAL);
    ui_graph_render_indicators(&cpugrap, element_get_surface_ptr(cpugraph_elem, 0), (point){0, 6}, 0x000f);
    ui_graph_render_indicators(&ramgrap, element_get_surface_ptr(ramgraph_elem, 0), (point){0, 6}, 0x000f);
    
    
    LARGE_INTEGER freq, start, current;
    double fps;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    MEMORYSTATUSEX memstatus;
    memstatus.dwLength = sizeof(MEMORYSTATUSEX);
    while (true) {
        ftime(&current_time);
        if (current_time.time > start_graph.time || current_time.millitm - start_graph.millitm > WAITTIME) {
            GlobalMemoryStatusEx(&memstatus);
            add_entry(&cpugrap, cpugraph_elem, (pointf){++x, GetCPULoad() * 100}, 1);
            add_entry(&ramgrap, ramgraph_elem, (pointf){x, memstatus.dwMemoryLoad}, 1);
            start_graph = current_time;
        }

        QueryPerformanceCounter(&current);
        double delta = (double)(current.QuadPart - start.QuadPart) / (double)freq.QuadPart;
        if (delta > 0.0) {
            fps = 1.0 / delta;
            char buf[10];
            snprintf(buf, sizeof(buf), "%d FPS", (int)fps);
            surface_clear(active_surface_ptr(fps_elem));
            surface *tmps = string_to_surface_char(buf, 0x000e);
            surface_paste(tmps, active_surface_ptr(fps_elem), (point){4, 2});
            free(tmps);
        }
        start = current;


        display_clear();
        display_update();
        display_draw();
    }
}
