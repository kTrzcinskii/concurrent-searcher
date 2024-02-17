#ifndef IGNORE_ENTRY
#define IGNORE_ENTRY

#include <stdlib.h>

typedef struct ignore_entry
{
    char **entries;
    size_t count;
} ignore_entry_t;

ignore_entry_t ignore_entry_create(char *path);
void quicksort_string(char **array, size_t left, size_t right);
size_t partition_string(char **array, size_t left, size_t right);
void swap_string(char **s1, char **s2);
int ignore_entry_contains(ignore_entry_t ignore_entry, char *name);
void ignore_entry_destroy(ignore_entry_t ignore_entry);

#endif // IGNORE_ENTRY