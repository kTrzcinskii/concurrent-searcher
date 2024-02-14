#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "concurrent_searcher.h"
#include "error_handler.h"
#include "file_content.h"

int main(int argc, char **argv)
{
    concurrent_searcher_args_t args;
    read_arguments(argc, argv, &args);

    found_file_list_t file_list = found_file_list_init();

    thread_worker_args_t *worker_args;
    initialize_thread_worker_args(&worker_args, &args.dir_list, &file_list, args.recursively, args.phrase, args.threads_num);

    create_threads(worker_args, thread_function, args.threads_num);
    join_threads(worker_args, args.threads_num);
    destroy_thread_worker_args(worker_args, args.threads_num);

    print_output(&file_list, args.output_path);

    found_file_list_clear(&file_list);
    clear_arguments(&args);
    return EXIT_SUCCESS;
}

void usage(char *pname)
{
    fprintf(stderr, "USAGE: %s [-r] [-t threads_num] [-o output_path] [-p phrase] [-i input_path] [-d dir_path] directories\n", pname);
    fprintf(stderr, "(optional) r - search directories recursively\n");
    fprintf(stderr, "(optional) threads_num - number of threads to be created to concurrently search through directories. Default value is minimum from number of provided directories and max range [integer from range %d-%d]\n", MIN_THREADS_NUM, MAX_THREADS_NUM);
    fprintf(stderr, "(optional) output_path - path to file in which program result should be stored. When no path is provided stdout is used\n");
    fprintf(stderr, "phrase - phrase to be looked for inside every file in provided directories\n");
    fprintf(stderr, "input_path - path to file which content will be looked for inside every file in provided directories\n");
    fprintf(stderr, "(optional) dir_path - path fo file with directories (each one in new line) in which files should be checked. They are appended to list of other directories provided in command line\n");
    fprintf(stderr, "directories - paths to directories (separated by spaces) in which files should be checked.\n");
    fprintf(stderr, "Important note: if both phrase and input_path are provided then phrase is used. One of those argument must be passed in order to start program.");
    exit(EXIT_FAILURE);
}

// TODO: consider supporting symbolic links
void read_arguments(int argc, char **argv, concurrent_searcher_args_t *args)
{
    char c;
    int is_recursive = 0;
    size_t threads_num = 0;
    char *p = NULL;
    char *o = NULL;
    char *i = NULL;
    char *d = NULL;

    while ((c = getopt(argc, argv, "rt:p:o:i:d:")) != -1)
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
            p = strdup(optarg);
            if (!p)
                ERR("strdup", GENERAL_ERROR);
            break;
        case 'o':
            o = strdup(optarg);
            if (!o)
                ERR("strdup", GENERAL_ERROR);
            break;
        case 'i':
            i = strdup(optarg);
            if (!i)
                ERR("strdup", GENERAL_ERROR);
            break;
        case 'd':
            d = strdup(optarg);
            if (!d)
                ERR("strdup", GENERAL_ERROR);
            break;
        case '?':
            usage(argv[0]);
            break;
        }
    }

    if (!p && !i)
        usage(argv[0]);

    int err;
    directories_list_t list = directory_list_init();
    for (int i = optind; i < argc; i++)
        if ((err = directory_list_push_back(&list, argv[i])) != 0)
            ERR("directory_list_push_back", err);

    if (d)
    {
        file_content_t dir_content = load_file(d, LOAD_MODE_REMOVE_N);
        for (size_t i = 0; i < dir_content.lines_num; i++)
            if ((err = directory_list_push_back(&list, dir_content.lines[i])) != 0)
                ERR("directory_list_push_back", err);
        file_content_clear(dir_content);
    }

    if (!p)
    {
        file_content_t input_content = load_file(i, LOAD_MODE_CHANGE_N_TO_SPACE);
        p = file_content_to_string(input_content);
        file_content_clear(input_content);
    }

    free(i);
    free(d);
    args->dir_list = list;
    args->recursively = is_recursive;
    args->threads_num = threads_num == 0 ? list.count : threads_num;
    args->phrase = p;
    args->output_path = o;
}

void clear_arguments(concurrent_searcher_args_t *args)
{
    directory_list_clear(&args->dir_list);
    free(args->phrase);
    free(args->output_path);
}

void print_output(found_file_list_t *list, char *output_path)
{
    FILE *output_stream = NULL;
    if (!output_path)
        output_stream = DEFAULT_OUTPUT_STREAM;
    else
    {
        output_stream = fopen(output_path, "w");
        if (!output_stream)
            handle_file_open_error(output_path);
    }

    fprintf(output_stream, "%ld\n", list->count);
    found_file_node_t *current = list->head;
    while (current)
    {
        fprintf(output_stream, "%s %ld:%ld-%ld:%ld\n", current->path, current->start_position.line + 1, current->start_position.column + 1, current->end_position.line + 1, current->end_position.column + 1);
        current = current->next;
    }

    if (output_path && fclose(output_stream))
        handle_file_close_error(output_path);
}

void initialize_thread_worker_args(thread_worker_args_t **args, directories_list_t *dir_list, found_file_list_t *file_list, int recursively, char *phrase, size_t threads_num)
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

    *args = malloc(sizeof(thread_worker_args_t) * threads_num);
    if (!(*args))
    {
        free(mx_available_directory);
        free(mx_file_list);
        ERR("malloc", ALLOCATION_ERROR);
    }

    if (pthread_mutex_init(mx_available_directory, NULL))
        ERR("pthread_mutex_init", GENERAL_ERROR);
    if (pthread_mutex_init(mx_file_list, NULL))
        ERR("pthread_mutex_init", GENERAL_ERROR);

    directory_node_t **dir_head = malloc(sizeof(directory_node_t *));
    if (!dir_head)
        ERR("malloc", ALLOCATION_ERROR);

    *dir_head = dir_list->head;

    for (size_t i = 0; i < threads_num; i++)
    {
        char *p = strdup(phrase);
        if (!p)
            ERR("strdup", ALLOCATION_ERROR);
        (*args)[i].mx_available_directory = mx_available_directory;
        (*args)[i].mx_file_list = mx_file_list;
        (*args)[i].available_directory = dir_head;
        (*args)[i].file_list = file_list;
        (*args)[i].recursively = recursively;
        (*args)[i].phrase = p;
    }
}

void destroy_thread_worker_args(thread_worker_args_t *args, size_t threads_num)
{
    if (pthread_mutex_destroy(args[0].mx_available_directory))
        ERR("pthread_mutex_destroy", GENERAL_ERROR);
    if (pthread_mutex_destroy(args[0].mx_file_list))
        ERR("pthread_mutex_destroy", GENERAL_ERROR);
    free(args[0].mx_available_directory);
    free(args[0].mx_file_list);
    free(args[0].available_directory);

    for (size_t i = 0; i < threads_num; i++)
        free(args[i].phrase);

    free(args);
}

void create_threads(thread_worker_args_t *worker_args, void *(*start_function)(void *), size_t thread_number)
{
    for (size_t i = 0; i < thread_number; i++)
        if (pthread_create(&worker_args[i].tid, NULL, start_function, (void *)&worker_args[i]))
            ERR("pthread_create", GENERAL_ERROR);
}

void join_threads(thread_worker_args_t *worker_args, size_t thread_number)
{
    for (size_t i = 0; i < thread_number; i++)
        if (pthread_join(worker_args[i].tid, NULL))
            ERR("pthread_join", GENERAL_ERROR);
}

void *thread_function(void *argp)
{
    // cast
    thread_worker_args_t *args = argp;

    // search next available path
    directory_node_t *current_dir = NULL;
    do
    {
        if (pthread_mutex_lock(args->mx_available_directory))
            ERR("pthread_mutex_lock", GENERAL_ERROR);
        current_dir = *(args->available_directory);
        if (current_dir)
            *(args->available_directory) = current_dir->next;
        if (pthread_mutex_unlock(args->mx_available_directory))
            ERR("ptrhead_mutex_unlock", GENERAL_ERROR);
        if (current_dir)
            search_directory(current_dir->path, args->file_list, args->mx_file_list, args->recursively, args->phrase);
    } while (current_dir);

    return NULL;
}

void search_directory(char *directory_path, found_file_list_t *file_list, pthread_mutex_t *mx_file_list, int recursively, char *phrase)
{
    DIR *dir_stream = opendir(directory_path);
    if (!dir_stream)
        handle_dir_open_error(directory_path);

    directories_list_t dir_list;
    if (recursively)
        dir_list = directory_list_init();

    struct dirent *dir_entry;
    struct stat stat_buffer;
    do
    {
        errno = 0;
        if ((dir_entry = readdir(dir_stream)) != NULL)
        {
            char *entry_path_name = combine_paths(directory_path, dir_entry->d_name);
            if (stat(entry_path_name, &stat_buffer))
                ERR("stat", GENERAL_ERROR);

            if (S_ISREG(stat_buffer.st_mode))
                check_file(entry_path_name, file_list, mx_file_list, phrase);

            if (recursively && S_ISDIR(stat_buffer.st_mode) && strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, ".."))
                directory_list_push_back(&dir_list, entry_path_name);

            free(entry_path_name);
        }
    } while (dir_entry != NULL);

    if (errno)
        ERR("readdir", GENERAL_ERROR);

    if (closedir(dir_stream))
        handle_dir_close_error(directory_path);

    if (recursively)
    {
        directory_node_t *current = dir_list.head;
        while (current)
        {
            search_directory(current->path, file_list, mx_file_list, recursively, phrase);
            current = current->next;
        }
        directory_list_clear(&dir_list);
    }
}

void check_file(char *file_path, found_file_list_t *file_list, pthread_mutex_t *mx_file_list, char *phrase)
{
    file_content_t file_content = load_file(file_path, LOAD_MODE_CHANGE_N_TO_SPACE);

    size_t positions_count = 0;
    file_position_t *starting_positions = find_in_file_kmp(file_content, phrase, &positions_count);
    if (positions_count == 0)
    {
        file_content_clear(file_content);
        return;
    }

    file_position_t *ending_positions = malloc(sizeof(file_position_t) * positions_count);
    for (size_t i = 0; i < positions_count; i++)
        ending_positions[i] = index_to_position(file_content, position_to_index(file_content, starting_positions[i]) + strlen(phrase) - 1);

    if (pthread_mutex_lock(mx_file_list))
        ERR("pthread_mutex_lock", GENERAL_ERROR);
    for (size_t i = 0; i < positions_count; i++)
        found_file_list_push_back(file_list, file_path, starting_positions[i], ending_positions[i]);
    if (pthread_mutex_unlock(mx_file_list))
        ERR("pthread_mutex_unlock", GENERAL_ERROR);

    file_content_clear(file_content);
    free(starting_positions);
    free(ending_positions);
}

char *combine_paths(char *p1, char *p2)
{
    char *p = malloc(sizeof(char) * (strlen(p1) + strlen(p2) + 2));
    strcpy(p, p1);
    strcat(p, "/");
    strcat(p, p2);
    return p;
}
