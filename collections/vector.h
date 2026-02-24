#ifndef VECTOR_FOR_MY_SHIT
#define VECTOR_FOR_MY_SHIT

#define MAKE_PTRVECTOR(T)   \
typedef struct pvector_##T  \
{                           \
    T *data;                \
    size_t capacity;        \
    size_t count;           \
} pvector_##T;              \
\
static inline void pvector_##T##_init(pvector_##T *v) { \
    v->count = 0;                                       \
    v->capacity = 8;                                    \
    v->data = malloc(sizeof(T) * 8);                    \
}                                                       \
\
static inline void pvector_##T##_push(pvector_##T *v, T value) {    \
    if (v->count == v->capacity) {                                  \
        v->capacity *= 2;                                           \
        v->data = realloc(v->data, sizeof(T) * v->capacity);        \
    }                                                               \
    v->data[v->count++] = value;                                    \
}                                                                   \
\
static inline void pvector_##T##_remove(pvector_##T *v, size_t index) { \
    if (index >= v->count) return;                                      \
                                                                        \
    free(v->data[index]);                                               \
    if (index == v->count-1) {                                          \
        v->count--;                                                     \
        return;                                                         \
    }                                                                   \
                                                                        \
    for (size_t i = index; i < v->count-1; i++)                         \
    {                                                                   \
        v->data[i] = v->data[i+1];                                      \
    }                                                                   \
    v->count--;                                                         \
}                                                                       \
\
static inline void pvector_##T##_remove_unordered(pvector_##T *v, size_t index) {   \
    if (index >= v->count) return;                                                  \
                                                                                    \
    free(v->data[index]);                                                           \
    v->data[index] = v->data[--(v->count)];                                         \
}                                                                                   \
\
static inline void pvector_##T##_destroy(pvector_##T *v) {  \
    for (size_t i = 0; i < v->count; i++)                   \
    {                                                       \
        free(v->data[i]);                                   \
    }                                                       \
    free(v->data);                                          \
    v->data = 0;                                            \
    v->count = 0;                                           \
    v->capacity = 0;                                        \
}                                                           \
\
static inline void pvector_##T##_clear(pvector_##T *v) {    \
    for (size_t i = 0; i < v->count; i++) {                 \
        free(v->data[i]);                                   \
        v->data[i] = 0;                                     \
    }                                                       \
    v->count = 0;                                           \
}



#define MAKE_VECTOR(T)      \
typedef struct vector_##T   \
{                           \
    T *data;                \
    size_t capacity;        \
    size_t count;           \
} vector_##T;               \
\
static inline void vector_##T##_init(vector_##T *v) {   \
    v->count = 0;                                       \
    v->capacity = 10;                                   \
    v->data = malloc(sizeof(T) * 10);                   \
}                                                       \
\
static inline void vector_##T##_push(vector_##T *v, T value) {  \
    if (v->count == v->capacity) {                              \
        v->capacity *= 2;                                       \
        v->data = realloc(v->data, sizeof(T) * v->capacity);    \
    }                                                           \
    v->data[v->count++] = value;                                \
}                                                               \
\
static inline void vector_##T##_remove(vector_##T *v, size_t index) {   \
    if (index >= v->count) return;                                      \
                                                                        \
    if (index == v->count-1) {                                          \
        v->count--;                                                     \
        return;                                                         \
    }                                                                   \
                                                                        \
    for (size_t i = index; i < v->count-1; i++)                         \
    {                                                                   \
        v->data[i] = v->data[i+1];                                      \
    }                                                                   \
    v->count--;                                                         \
}                                                                       \
\
static inline void vector_##T##_remove_unordered(vector_##T *v, size_t index) { \
    if (index >= v->count) return;                                              \
    v->data[index] = v->data[--(v->count)];                                     \
}                                                                               \
\
static inline void vector_##T##_destroy(vector_##T *v) {    \
    free(v->data);                                          \
    v->data = 0;                                            \
    v->count = 0;                                           \
    v->capacity = 0;                                        \
}                                                           \
\
static inline void vector_##T##_clear(vector_##T *v) {  \
    v->count = 0;                                       \
}                                                       \
\
static inline size_t vector_##T##_find_index(vector_##T *v, int (*matchfn)(T)) {    \
    for (int i = 0; i < v->count; i++) {                                            \
        if (matchfn(v->data[i]))                                                    \
            return i;                                                               \
    }                                                                               \
    return SIZE_MAX;                                                                \
}

#endif
