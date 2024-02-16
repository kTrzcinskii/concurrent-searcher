#ifndef ENTRY_LIST
#define ENTRY_LIST

#include <stdlib.h>

typedef struct entry_node
{
    char *path;
    struct entry_node *next;
} entry_node_t;

typedef struct entry_list
{
    entry_node_t *head;
    entry_node_t *tail;
    size_t count;
} entry_list_t;

entry_list_t entry_list_init();
int entry_list_push_back(entry_list_t *list, const char *path);
entry_node_t *entry_list_pop_first(entry_list_t *list);
void entry_list_clear(entry_list_t *list);

#endif // ENTRY_LIST