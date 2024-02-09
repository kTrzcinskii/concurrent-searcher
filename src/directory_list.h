#ifndef DIRECTORY_LIST
#define DIRECTORY_LIST

#include <stdlib.h>

typedef struct directory_node
{
    char *path;
    struct directory_node *next;
} directory_node_t;

typedef struct directories_list
{
    directory_node_t *head;
    directory_node_t *tail;
    size_t count;
} directories_list_t;

directories_list_t directory_list_init();
int directory_list_push_back(directories_list_t *list, const char *path);
directory_node_t *directory_list_pop_first(directories_list_t *list);
void directory_list_clear(directories_list_t *list);

#endif // DIRECTORY_LIST