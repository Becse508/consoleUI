#include "list.h"

void list_free(list *collection, bool freeValues) {
    list_node *current = collection->head;
    list_node *prev = 0;

    while (current)
    {
        prev = current;
        current = current->next;

        if (freeValues)
            free(prev->value);
        free(prev);
    }

    collection->head = 0;
    collection->tail = 0;
    collection->size = 0;
}


void* list_get_node(list *collection, size_t index) {
    if (!collection || !collection->head || index >= collection->size)
        return 0;

    list_node *current = collection->head;

    for (size_t i = 0; i < index; i++)
    {
        if (!current->next) return 0;
        current = current->next;
    }
    
    return current;
}
void* list_get(list *collection, size_t index) {
    list_node *node = list_get_node(collection, index);
    if (!node) return 0;
    return node->value;
}

bool list_push(list *collection, void *value) {
    if (!collection)
        return false;

    list_node *new_head = malloc(sizeof(list_node));
    if (!new_head) return false;

    new_head->value = value;
    new_head->next = collection->head; 
    
    if (!collection->tail) {
        collection->tail = new_head;
    }

    collection->head = new_head;
    collection->size++;
        
    return true;
}

/// @brief Append multiple values to the list. Function quits if one value fails.
/// @param count number of values to append
/// @return The number of values successfully appended.
size_t list_append_many(list *collection, size_t count, ...) {
    va_list values;
    va_start(values, count);

    size_t i = 0;
    for (i = 0; i < count; i++)
    {
        if (!list_append(collection, va_arg(values, void *))) {
            va_end(values);
            return i;
        }
    }

    va_end(values);
    return i;
}

bool list_append(list *collection, void *value) {
    if (!collection)
        return false;
    
    list_node *new_tail = malloc(sizeof(list_node));
    if (!new_tail) return false;

    new_tail->value = value;
    new_tail->next = 0;

    if (!collection->tail) {
        collection->head = new_tail;
    } else {
        collection->tail->next = new_tail;
    }

    collection->tail = new_tail;
    collection->size++;
    return true;
}

void* list_remove_first(list *collection) {
    if (!collection || !collection->head) 
        return 0;

    list_node *old_head = collection->head;
    void *value = old_head->value;
    
    collection->head = old_head->next;

    if (collection->size == 1) {
        collection->tail = 0;
    }

    collection->size--;
    free(old_head);

    return value;
}

void* list_remove(list *collection, size_t index) {
    if (!collection || !collection->head || index >= collection->size) 
        return 0;

    list_node *current = collection->head;

    if (index == 0) {
        return list_remove_first(collection);
    }

    for (size_t i = 0; i < index-1; i++)
    {
        if (!current->next) return 0;
        current = current->next;
    }

    list_node* to_delete = current->next;
    void* value = to_delete->value;
    current->next = to_delete->next;

    collection->size--;
    free(to_delete);

    return value;
}

void* list_remove_last(list *collection) {
    if (!collection || collection->size == 0) return 0;
    return list_remove(collection, collection->size-1);
}
