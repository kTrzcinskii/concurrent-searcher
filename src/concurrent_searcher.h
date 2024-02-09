#ifndef CONCURRENT_SEARCHER
#define CONCURRENT_SEARCHER

#include <stdlib.h>

#include "directory_list.h"

typedef struct concurrent_searcher_args
{
    directories_list_t dir_list;
    int recursively;
} concurrent_searcher_args_t;

void usage(char *pname);
void read_arguments(int argc, char **argv, concurrent_searcher_args_t *args);

#endif // CONCURRENT_SEARCHER