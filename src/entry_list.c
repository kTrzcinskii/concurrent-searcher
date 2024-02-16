#include <string.h>

#include "entry_list.h"
#include "error_handler.h"

entry_list_t entry_list_init()
{
    entry_list_t list;
    list.count = 0;
    list.head = NULL;
    list.tail = NULL;
    return list;
}

int entry_list_push_back(entry_list_t *list, const char *path)
{
    if (!list)
        return GENERAL_ERROR;

    entry_node_t *node = malloc(sizeof(entry_node_t));
    if (!node)
        return ALLOCATION_ERROR;

    char *node_path = strdup(path);
    if (!node_path)
    {
        free(node);
        return GENERAL_ERROR;
    }

    node->path = node_path;
    node->next = NULL;

    if (list->head)
    {
        list->tail->next = node;
        list->tail = node;
    }
    else
    {
        list->head = node;
        list->tail = node;
    }
    list->count++;

    return 0;
}

entry_node_t *entry_list_pop_first(entry_list_t *list)
{
    if (!list || list->count == 0)
        return NULL;
    entry_node_t *tmp = list->head;
    list->head = list->head->next;
    list->count--;
    if (!list->head)
        list->tail = NULL;
    tmp->next = NULL;
    return tmp;
}

void entry_list_clear(entry_list_t *list)
{
    if (!list)
        return;
    while (list->count > 0)
    {
        entry_node_t *tmp = entry_list_pop_first(list);
        free(tmp->path);
        free(tmp);
    }
}
