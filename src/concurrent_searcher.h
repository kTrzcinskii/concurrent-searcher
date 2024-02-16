#ifndef CONCURRENT_SEARCHER
#define CONCURRENT_SEARCHER

#include <pthread.h>
#include <stdlib.h>

#include "directory_list.h"
#include "found_files_list.h"

#define MIN_THREADS_NUM 1
#define MAX_THREADS_NUM 16

#define DEFAULT_OUTPUT_STREAM stdout

#define CURRENT_DIR_PATH_BUFF 1024

typedef struct concurrent_searcher_args
{
    directories_list_t dir_list;
    char *phrase;
    size_t threads_num;
    int recursively;
    char *output_path;
    int follow_symlinks;
} concurrent_searcher_args_t;

typedef struct thread_worker_args
{
    pthread_t tid;
    directory_node_t **available_directory;
    pthread_mutex_t *mx_available_directory;
    found_file_list_t *file_list;
    pthread_mutex_t *mx_file_list;
    int recursively;
    char *phrase;
    int follow_symlinks;
} thread_worker_args_t;

void usage(char *pname);
void read_arguments(int argc, char **argv, concurrent_searcher_args_t *args);
void clear_arguments(concurrent_searcher_args_t *args);
void print_output(found_file_list_t *list, char *output_path);
void initialize_thread_worker_args(thread_worker_args_t **args, directories_list_t *dir_list, found_file_list_t *file_list, int recursively, char *phrase, size_t threads_num, int follow_symlinks);
void destroy_thread_worker_args(thread_worker_args_t *args, size_t threads_num);
void create_threads(thread_worker_args_t *worker_args, void *(*start_function)(void *), size_t threads_num);
void join_threads(thread_worker_args_t *threads, size_t threads_num);
void *thread_function(void *argp);
void search_directory(char *directory_path, found_file_list_t *file_list, pthread_mutex_t *mx_file_list, int recursively, char *phrase, int follow_symlinks);
void check_file(char *file_path, found_file_list_t *file_list, pthread_mutex_t *mx_file_list, char *phrase);
char *combine_paths(char *p1, char *p2);

#endif // CONCURRENT_SEARCHER