#ifndef CONCURRENT_SEARCHER
#define CONCURRENT_SEARCHER

#include <stdlib.h>

#include "directory_list.h"
#include "found_files_list.h"

#define MIN_THREADS_NUM 1
#define MAX_THREADS_NUM 16

#define DEFAULT_OUTPUT_STREAM stdout

typedef struct concurrent_searcher_args
{
    directories_list_t dir_list;
    char *phrase;
    size_t threads_num;
    int recursively;
} concurrent_searcher_args_t;

void usage(char *pname);
void read_arguments(int argc, char **argv, concurrent_searcher_args_t *args);
void clear_arguments(concurrent_searcher_args_t *args);
void print_output(found_file_list_t *list, char *output_path);

#endif // CONCURRENT_SEARCHER