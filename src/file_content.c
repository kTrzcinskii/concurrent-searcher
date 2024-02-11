#include <string.h>

#include "error_handler.h"
#include "file_content.h"

file_content_t load_file(char *file_path)
{
    if (!file_path)
        ERR("empty file_path", GENERAL_ERROR);
    FILE *f = fopen(file_path, "r");
    if (!f)
        handle_file_open_error(file_path);

    char **lines = malloc(sizeof(char *));
    if (!lines)
        ERR("malloc", ALLOCATION_ERROR);
    size_t lines_num = 0;
    size_t characters_num = 0;
    size_t lines_capacity = 1;

    char *line = NULL;
    size_t line_len = 0;
    while (getline(&line, &line_len, f) != -1)
    {
        if (lines_num == lines_capacity)
        {
            lines_capacity *= 2;
            lines = realloc(lines, sizeof(char *) * lines_capacity);
            if (!lines)
                ERR("realloc", GENERAL_ERROR);
        }
        line[strlen(line) - 1] = '\0'; // remove endline from the line
        char *p = malloc(sizeof(char) * (strlen(line) + 1));
        if (!p)
            ERR("malloc", ALLOCATION_ERROR);
        strcpy(p, line);
        lines[lines_num++] = p;
        characters_num += strlen(line);
    }
    free(line);

    if (fclose(f))
        handle_file_close_error(file_path);

    file_content_t file;
    file.lines = lines;
    file.lines_num = lines_num;
    file.characters_num = characters_num;

    return file;
}

char file_content_at(file_content_t file, size_t index)
{
    // TODO:
    return 0;
}

size_t position_to_index(file_content_t file, file_position_t file_position)
{
    // TODO:
    return 0;
}

file_position_t index_to_position(file_content_t file, size_t index)
{
    // TODO:
    file_position_t position = {.column = -1, .line = -1};
    return position;
}

void file_content_clear(file_content_t file)
{
    for (size_t i = 0; i < file.lines_num; i++)
        free(file.lines[i]);
    free(file.lines);
}

size_t *create_lps_table(char *phrase)
{
    // TODO:
    return NULL;
}

file_position_t *find_in_file_kmp(file_content_t file, char *phrase)
{
    // TODO:
    return NULL;
}
