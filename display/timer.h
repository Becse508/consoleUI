#ifndef TIMER_HEADER_AI_GENERATED_I_WAS_TOO_LAZY_TO_MAKE_IT
#define TIMER_HEADER_AI_GENERATED_I_WAS_TOO_LAZY_TO_MAKE_IT

#ifdef _WIN32
    #include <windows.h>
#else
    #include <time.h>
#endif

typedef struct {
#ifdef _WIN32
    LARGE_INTEGER start;
#else
    struct timespec start;
#endif
} timer_t;

#ifdef _WIN32

static LARGE_INTEGER _timer_freq;
static int _timer_initialized = 0;

static inline void timer_init(void) {
    if (!_timer_initialized) {
        QueryPerformanceFrequency(&_timer_freq);
        _timer_initialized = 1;
    }
}

static inline void timer_start(timer_t* t) {
    QueryPerformanceCounter(&t->start);
}

static inline double timer_elapsed(const timer_t* t) {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - t->start.QuadPart) /
           (double)_timer_freq.QuadPart;
}

#else

static inline void timer_init(void) { }

static inline void timer_start(timer_t* t) {
    clock_gettime(CLOCK_MONOTONIC, &t->start);
}

static inline double timer_elapsed(const timer_t* t) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    return (now.tv_sec - t->start.tv_sec) +
           (now.tv_nsec - t->start.tv_nsec) * 1e-9;
}

#endif

#endif
