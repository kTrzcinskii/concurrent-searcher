#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "concurrent_searcher.h"
#include "error_handler.h"

int main(int argc, char **argv)
{
    concurrent_searcher_args_t args;
    read_arguments(argc, argv, &args);

    clear_arguments(&args);
    return EXIT_SUCCESS;
}

void usage(char *pname)
{
    fprintf(stderr, "USAGE: %s [-r] [-t threads_num] -p phrase directories\n", pname);
    fprintf(stderr, "r - search directories recursively\n");
    fprintf(stderr, "threads_num - number of threads to be created to concurrently search through directories. Default value is minimum from number of provided directories and max range [integer from range %d-%d]\n", MIN_THREADS_NUM, MAX_THREADS_NUM);
    fprintf(stderr, "phrase - phrase to be looked for inside every file in provided directories\n");
    fprintf(stderr, "directories - path to directories (separated by spaces) in which files should be checked\n");
    exit(EXIT_FAILURE);
}

// TODO: add option to get phrase from file
// TODO: add option to get directories from file
// TODO: add option to get output to the file
void read_arguments(int argc, char **argv, concurrent_searcher_args_t *args)
{
    char c;
    int is_recursive = 0;
    size_t threads_num = 0;
    char *p = NULL;

    while ((c = getopt(argc, argv, "rt:p:")) != -1)
    {
        switch (c)
        {
        case 'r':
            is_recursive = 1;
            break;
        case 't':
            threads_num = atoi(optarg);
            if (threads_num < MIN_THREADS_NUM || threads_num > MAX_THREADS_NUM)
                usage(argv[0]);
            break;
        case 'p':
            p = malloc(sizeof(char) * (strlen(optarg) + 1));
            if (!p)
                ERR("malloc", ALLOCATION_ERROR);
            strcpy(p, optarg);
            break;
        case '?':
            usage(argv[0]);
            break;
        }
    }

    if (!p)
        usage(argv[0]);

    int err;
    directories_list_t list = directory_list_init();
    for (int i = optind; i < argc; i++)
        if ((err = directory_list_push_back(&list, argv[i])) != 0)
            ERR("directory_list_push_back", err);

    args->dir_list = list;
    args->recursively = is_recursive;
    args->threads_num = threads_num == 0 ? list.count : threads_num;
    args->phrase = p;
}

void clear_arguments(concurrent_searcher_args_t *args)
{
    directory_list_clear(&args->dir_list);
    free(args->phrase);
}
