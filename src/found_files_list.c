#include <string.h>

#include "error_handler.h"
#include "found_files_list.h"

found_file_list_t found_file_list_init()
{
    found_file_list_t list;
    list.count = 0;
    list.head = NULL;
    list.tail = NULL;
    return list;
}

int found_file_list_push_back(found_file_list_t *list, char *path, file_position_t start, file_position_t end)
{
    if (!list)
        return GENERAL_ERROR;

    found_file_node_t *node = malloc(sizeof(found_file_node_t));
    if (!node)
        return ALLOCATION_ERROR;

    char *p = strdup(path);
    if (!p)
    {
        free(node);
        return GENERAL_ERROR;
    }

    node->path = p;
    node->next = NULL;
    node->start_position = start;
    node->end_position = end;

    if (!list->head)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }
    list->count++;

    return 0;
}

found_file_node_t *found_file_list_pop_first(found_file_list_t *list)
{
    if (!list || list->count == 0)
        return NULL;

    found_file_node_t *tmp = list->head;
    list->head = list->head->next;
    if (!list->head)
        list->tail = NULL;
    list->count--;

    tmp->next = NULL;
    return tmp;
}

void found_file_list_clear(found_file_list_t *list)
{
    if (!list)
        return;
    while (list->count > 0)
    {
        found_file_node_t *tmp = found_file_list_pop_first(list);
        free(tmp->path);
        free(tmp);
    }
}
