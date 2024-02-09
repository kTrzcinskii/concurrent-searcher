#ifndef CONCURRENT_SEARCHER
#define CONCURRENT_SEARCHER

#include <pthread.h>
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

typedef struct thread_worker_args
{
    directory_node_t *available_directory;
    pthread_mutex_t *mx_available_directory;
    found_file_list_t *file_list;
    pthread_mutex_t *mx_file_list;
    int recursively;
    pthread_mutex_t *mx_recursively;
} thread_worker_args_t;

void usage(char *pname);
void read_arguments(int argc, char **argv, concurrent_searcher_args_t *args);
void clear_arguments(concurrent_searcher_args_t *args);
void print_output(found_file_list_t *list, char *output_path);
void initialize_thread_worker_args(thread_worker_args_t *args, directories_list_t *dir_list, found_file_list_t *file_list, int recursively);
void destroy_thread_worker_args(thread_worker_args_t *args);
void create_threads(pthread_t **threads, void *(*start_function)(void *), void *thread_args, size_t thread_number);
void join_threads(pthread_t *threads, size_t thread_number);
void *thread_function(void *argp);

#endif // CONCURRENT_SEARCHER