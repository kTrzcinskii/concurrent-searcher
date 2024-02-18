#include <errno.h>

#include "error_handler.h"
#include "file_content.h"
#include "ignore_entry.h"

ignore_entry_t ignore_entry_create(char *path)
{
    ignore_entry_t ignore_entry;

    file_content_t content = load_file(path, LOAD_MODE_REMOVE_N);
    ino_t *entries = malloc(sizeof(ino_t) * content.lines_num);
    if (!entries)
        ERR("malloc", ALLOCATION_ERROR);
    struct stat st_buffer;
    for (size_t i = 0; i < content.lines_num; i++)
    {
        if (stat(content.lines[i], &st_buffer))
            ERR("stat", GENERAL_ERROR);
        entries[i] = st_buffer.st_ino;
    }

    ignore_entry.entries = entries;
    ignore_entry.count = content.lines_num;
    file_content_clear(content);

    quicksort_ino(ignore_entry.entries, 0, ignore_entry.count - 1);
    return ignore_entry;
}

void quicksort_ino(ino_t *array, size_t left, size_t right)
{
    while (right > left)
    {
        size_t j = partition_ino(array, left, right);
        if (j - left < right - j)
        {
            quicksort_ino(array, left, j - 1);
            left = j + 1;
        }
        else
        {
            quicksort_ino(array, j + 1, right);
            right = j - 1;
        }
    }
}

size_t partition_ino(ino_t *array, size_t left, size_t right)
{
    ino_t v = array[right];

    size_t i = left - 1;
    for (size_t j = left; j < right; j++)
    {
        if (array[j] <= v)
        {
            i++;
            if (i != j)
                swap_ino(&array[i], &array[j]);
        }
    }

    i++;
    swap_ino(&array[i], &array[right]);

    return i;
}

void swap_ino(ino_t *s1, ino_t *s2)
{
    ino_t tmp;
    tmp = *s1;
    *s1 = *s2;
    *s2 = tmp;
}

int ignore_entry_contains(ignore_entry_t ignore_entry, char *name)
{
    struct stat st_buffer;
    if (stat(name, &st_buffer))
    {
        // file doesnt exist
        if (errno == ENOENT)
            return 0;
        else
            ERR("stat", GENERAL_ERROR);
    }

    ino_t ino = st_buffer.st_ino;
    fprintf(stderr, "\n[entry] found ino: %ld\n", ino);

    size_t left = 0;
    size_t right = ignore_entry.count - 1;
    size_t mid;
    while (left < right)
    {
        mid = (left + right) / 2;
        if (ino == ignore_entry.entries[mid])
            return 1;
        if (ino < ignore_entry.entries[mid])
            right = mid - 1;
        else
            left = mid + 1;
    }
    if (ino == ignore_entry.entries[left])
        return 1;
    return 0;
}

void ignore_entry_destroy(ignore_entry_t ignore_entry)
{
    free(ignore_entry.entries);
}
