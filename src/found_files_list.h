#ifndef FOUND_FILES_LIST
#define FOUND_FILES_LIST

#include <stdlib.h>

typedef struct file_position
{
    int line;
    int column;
} file_position_t;

typedef struct found_file_node
{
    struct found_file_node *next;
    char *path;
    file_position_t start_position;
    file_position_t end_position;
} found_file_node_t;

typedef struct found_file_list
{
    found_file_node_t *head;
    found_file_node_t *tail;
    size_t count;
} found_file_list_t;

found_file_list_t found_file_list_init();
int found_file_list_push_back(found_file_list_t *list, char *path, file_position_t start, file_position_t end);
found_file_node_t *found_file_list_pop_first(found_file_list_t *list);
void found_file_list_clear(found_file_list_t *list);

#endif // FOUND_FILES_LIST