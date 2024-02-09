#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "concurrent_searcher.h"
#include "error_handler.h"

int main(int argc, char **argv)
{
    concurrent_searcher_args_t args;
    read_arguments(argc, argv, &args);

    found_file_list_t file_list = found_file_list_init();

    thread_worker_args_t worker_args;
    initialize_thread_worker_args(&worker_args, &args.dir_list, &file_list, args.recursively);

    pthread_t *threads = NULL;
    create_threads(&threads, thread_function, (void *)&worker_args, args.threads_num);
    join_threads(threads, args.threads_num);

    // TODO: remove hardcoded NULL with user defined option
    print_output(&file_list, NULL);

    destroy_thread_worker_args(&worker_args);
    found_file_list_clear(&file_list);
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

void print_output(found_file_list_t *list, char *output_path)
{
    FILE *output_stream = NULL;
    if (!output_path)
        output_stream = DEFAULT_OUTPUT_STREAM;
    else
    {
        output_stream = fopen(output_path, "r");
        if (!output_stream)
            handle_file_open_error(output_path);
    }

    found_file_node_t *current = list->head;
    while (current)
    {
        fprintf(output_stream, "%s %d:%d-%d:%d", current->path, current->start_position.line, current->start_position.column, current->end_position.line, current->end_position.column);
        current = current->next;
    }

    if (output_path && fclose(output_stream))
        handle_file_close_error(output_path);
}

void initialize_thread_worker_args(thread_worker_args_t *args, directories_list_t *dir_list, found_file_list_t *file_list, int recursively)
{
    pthread_mutex_t *mx_available_directory = malloc(sizeof(pthread_mutex_t));
    if (!mx_available_directory)
        ERR("malloc", ALLOCATION_ERROR);
    pthread_mutex_t *mx_file_list = malloc(sizeof(pthread_mutex_t));
    if (!mx_file_list)
    {
        free(mx_available_directory);
        ERR("malloc", ALLOCATION_ERROR);
    }
    pthread_mutex_t *mx_recursively = malloc(sizeof(pthread_mutex_t));
    if (!mx_recursively)
    {
        free(mx_available_directory);
        free(mx_file_list);
        ERR("malloc", ALLOCATION_ERROR);
    }

    if (pthread_mutex_init(mx_available_directory, NULL))
        ERR("pthread_mutex_init", GENERAL_ERROR);
    if (pthread_mutex_init(mx_file_list, NULL))
        ERR("pthread_mutex_init", GENERAL_ERROR);
    if (pthread_mutex_init(mx_recursively, NULL))
        ERR("pthread_mutex_init", GENERAL_ERROR);

    args->mx_available_directory = mx_available_directory;
    args->mx_file_list = mx_file_list;
    args->mx_recursively = mx_recursively;
    args->available_directory = dir_list->head;
    args->file_list = file_list;
    args->recursively = recursively;
}

void destroy_thread_worker_args(thread_worker_args_t *args)
{
    if (pthread_mutex_destroy(args->mx_available_directory))
        ERR("pthread_mutex_destroy", GENERAL_ERROR);
    if (pthread_mutex_destroy(args->mx_file_list))
        ERR("pthread_mutex_destroy", GENERAL_ERROR);
    if (pthread_mutex_destroy(args->mx_recursively))
        ERR("pthread_mutex_destroy", GENERAL_ERROR);
    free(args->mx_available_directory);
    free(args->mx_file_list);
    free(args->mx_recursively);
}

void create_threads(pthread_t **threads, void *(*start_function)(void *), void *thread_args, size_t thread_number)
{
    *threads = malloc(sizeof(pthread_t) * thread_number);
    if (!(*threads))
        ERR("malloc", ALLOCATION_ERROR);

    for (int i = 0; i < thread_number; i++)
        if (pthread_create(&(*threads)[i], NULL, start_function, thread_args))
            ERR("pthread_create", GENERAL_ERROR);
}

void join_threads(pthread_t *threads, size_t thread_number)
{
    for (int i = 0; i < thread_number; i++)
        if (pthread_join(threads[i], NULL))
            ERR("pthread_join", GENERAL_ERROR);
    free(threads);
}

// TODO: create actual function
void *thread_function(void *argp)
{
    printf("Welcome from working thread!\n");
    return NULL;
}
