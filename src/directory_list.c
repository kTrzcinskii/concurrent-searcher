#include <string.h>

#include "directory_list.h"
#include "error_handler.h"

directories_list_t directory_list_init()
{
    directories_list_t list;
    list.count = 0;
    list.head = NULL;
    list.tail = NULL;
    return list;
}

int directory_list_push_back(directories_list_t *list, const char *path)
{
    if (!list)
        return GENERAL_ERROR;

    directory_node_t *node = malloc(sizeof(directory_node_t));
    if (!node)
        return ALLOCATION_ERROR;

    char *node_path = malloc(sizeof(char) * (strlen(path) + 1));
    if (!node_path)
    {
        free(node);
        return ALLOCATION_ERROR;
    }
    strcpy(node_path, path);

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

directory_node_t *directory_list_pop_first(directories_list_t *list)
{
    if (!list || list->count == 0)
        return NULL;
    directory_node_t *tmp = list->head;
    list->head = list->head->next;
    list->count--;
    if (!list->head)
        list->tail = NULL;
    tmp->next = NULL;
    return tmp;
}

void directory_list_clear(directories_list_t *list)
{
    if (!list)
        return;
    while (list->count > 0)
    {
        directory_node_t *tmp = directory_list_pop_first(list);
        free(tmp->path);
        free(tmp);
    }
}
