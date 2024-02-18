#ifndef IGNORE_ENTRY
#define IGNORE_ENTRY

#include <stdlib.h>
#include <sys/stat.h>

typedef struct ignore_entry
{
    ino_t *entries;
    size_t count;
} ignore_entry_t;

ignore_entry_t ignore_entry_create(char *path);
void quicksort_ino(ino_t *array, size_t left, size_t right);
size_t partition_ino(ino_t *array, size_t left, size_t right);
void swap_ino(ino_t *s1, ino_t *s2);
int ignore_entry_contains(ignore_entry_t ignore_entry, char *name);
void ignore_entry_destroy(ignore_entry_t ignore_entry);

#endif // IGNORE_ENTRY