#include <string.h>

#include "error_handler.h"
#include "file_content.h"
#include "ignore_entry.h"

ignore_entry_t ignore_entry_create(char *path)
{
    ignore_entry_t ignore_entry;

    file_content_t content = load_file(path, LOAD_MODE_REMOVE_N);
    char **entries = malloc(sizeof(char *) * content.lines_num);
    if (!entries)
        ERR("malloc", ALLOCATION_ERROR);
    for (size_t i = 0; i < content.lines_num; i++)
    {
        char *s = strdup(content.lines[i]);
        if (!s)
            ERR("stdup", GENERAL_ERROR);
        entries[i] = s;
    }

    ignore_entry.entries = entries;
    ignore_entry.count = content.lines_num;
    file_content_clear(content);

    quicksort_string(ignore_entry.entries, 0, ignore_entry.count - 1);
    return ignore_entry;
}

void quicksort_string(char **array, size_t left, size_t right)
{
    while (right > left)
    {
        size_t j = partition_string(array, left, right);
        if (j - left < right - j)
        {
            quicksort_string(array, left, j - 1);
            left = j + 1;
        }
        else
        {
            quicksort_string(array, j + 1, right);
            right = j - 1;
        }
    }
}

size_t partition_string(char **array, size_t left, size_t right)
{
    char *v = array[right];

    size_t i = left - 1;
    for (size_t j = left; j < right; j++)
    {
        if (strcmp(array[j], v) <= 0)
        {
            i++;
            if (i != j)
                swap_string(&array[i], &array[j]);
        }
    }

    i++;
    swap_string(&array[i], &array[right]);

    return i;
}

void swap_string(char **s1, char **s2)
{
    char *tmp;
    tmp = *s1;
    *s1 = *s2;
    *s2 = tmp;
}

int ignore_entry_contains(ignore_entry_t ignore_entry, char *name)
{
    size_t left = 0;
    size_t right = ignore_entry.count - 1;
    size_t mid;
    while (left < right)
    {
        mid = (left + right) / 2;
        int result = strcmp(name, ignore_entry.entries[mid]);
        if (result == 0)
            return 1;
        if (result < 0)
            right = mid - 1;
        else
            left = mid + 1;
    }
    if (strcmp(name, ignore_entry.entries[left]) == 0)
        return 1;
    return 0;
}

void ignore_entry_destroy(ignore_entry_t ignore_entry)
{
    for (size_t i = 0; i < ignore_entry.count; i++)
        free(ignore_entry.entries[i]);
    free(ignore_entry.entries);
}
