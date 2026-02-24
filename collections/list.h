#ifndef GENERIC_LINKED_LIST_HEADER____HELL_YEAH
#define GENERIC_LINKED_LIST_HEADER____HELL_YEAH

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

typedef struct list_node {
    void *value;
    struct list_node *next;
} list_node;

/// @brief Always initialize to {0}
typedef struct list
{
    list_node *head;
    list_node *tail;
    size_t size;
} list;


/// @brief Frees all the memory used by the list
/// @param freeValues Only use if all values are on the heap!
void list_free(list *collection, bool freeValues);

void* list_get(list *collection, size_t index);

/// @brief Add new value at the start of a list
/// @return Success
bool list_push(list *collection, void *value);
/// @brief Add new value at the end of a list
/// @return Success
bool list_append(list *collection, void *value);
size_t list_append_many(list *collection, size_t count, ...);

void* list_remove_first(list *collection);
void* list_remove(list *collection, size_t index);
void* list_remove_last(list *collection);

#endif
