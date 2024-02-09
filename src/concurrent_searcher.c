#include <stdio.h>
#include <unistd.h>

#include "concurrent_searcher.h"
#include "error_handler.h"

int main(int argc, char **argv)
{
    concurrent_searcher_args_t args;
    read_arguments(argc, argv, &args);

    directory_list_clear(&args.dir_list);
    return EXIT_SUCCESS;
}

void usage(char *pname)
{
    fprintf(stderr, "USAGE: %s [-r] [-t threads_num] directories\n", pname);
    fprintf(stderr, "r - search directories recursively\n");
    fprintf(stderr, "threads_num - number of threads to be created to concurrently search through directories. Default value is minimum from number of provided directories and max range [integer from range %d-%d]\n", MIN_THREADS_NUM, MAX_THREADS_NUM);
    fprintf(stderr, "directories - path to directories (separated by spaces) in which files should be checked\n");
    exit(EXIT_FAILURE);
}

void read_arguments(int argc, char **argv, concurrent_searcher_args_t *args)
{
    char c;
    int is_recursive = 0;
    size_t threads_num = 0;
    while ((c = getopt(argc, argv, "rt:")) != -1)
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
        case '?':
            usage(argv[0]);
            break;
        }
    }

    int err;
    directories_list_t list = directory_list_init();
    for (int i = optind; i < argc; i++)
        if ((err = directory_list_push_back(&list, argv[i])) != 0)
            ERR("directory_list_push_back", err);

    args->dir_list = list;
    args->recursively = is_recursive;
    args->threads_num = threads_num == 0 ? list.count : threads_num;
}
