#ifndef FILE_CONTENT
#define FILE_CONTENT

#include <stdlib.h>

typedef struct file_position
{
    size_t line;
    size_t column;
} file_position_t;

typedef struct file_content
{
    char **lines;
    size_t lines_num;
    size_t characters_num;
} file_content_t;

typedef enum load_mode
{
    LOAD_MODE_BASIC,
    LOAD_MODE_CHANGE_N_TO_SPACE,
    LOAD_MODE_REMOVE_N
} load_mode_t;

file_content_t load_file(char *file_path, load_mode_t load_mode);
char file_content_at(file_content_t file, size_t index);
size_t position_to_index(file_content_t file, file_position_t file_position);
file_position_t index_to_position(file_content_t file, size_t index);
void file_content_clear(file_content_t file);
size_t *create_lps_table(char *phrase);
file_position_t *find_in_file_kmp(file_content_t file, char *phrase, size_t *count);
char *file_content_to_string(file_content_t file);

#endif // FILE_CONTENT