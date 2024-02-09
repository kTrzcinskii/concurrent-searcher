#ifndef CONCURRENT_SEARCHER
#define CONCURRENT_SEARCHER

#include <stdlib.h>

#include "directory_list.h"

#define MIN_THREADS_NUM 1
#define MAX_THREADS_NUM 16

typedef struct concurrent_searcher_args
{
    directories_list_t dir_list;
    size_t threads_num;
    int recursively;
} concurrent_searcher_args_t;

void usage(char *pname);
void read_arguments(int argc, char **argv, concurrent_searcher_args_t *args);

#endif // CONCURRENT_SEARCHER