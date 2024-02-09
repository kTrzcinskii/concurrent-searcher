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
    fprintf(stderr, "USAGE: %s [-r] directories\n", pname);
    fprintf(stderr, "r - search directories recursively\n");
    fprintf(stderr, "directories - path to directories (separated by spaces) in which files should be checked\n");
    exit(EXIT_FAILURE);
}

void read_arguments(int argc, char **argv, concurrent_searcher_args_t *args)
{
    char c;
    int is_recursive = 0;
    while ((c = getopt(argc, argv, "r")) != -1)
    {
        switch (c)
        {
        case 'r':
            is_recursive = 1;
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
}
