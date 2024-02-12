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

        // replace endline with spacebar
        // it will be useful when user pass phrase "some example"
        // and in the file we will have
        // "...... some"
        // "example ...."
        // we want to catch it, but we don't want to catch
        // "someexample"
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = ' ';

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
    file_position_t pos = index_to_position(file, index);
    if (pos.column == -1)
        ERR("index_to_position", GENERAL_ERROR);
    return file.lines[pos.line][pos.column];
}

size_t position_to_index(file_content_t file, file_position_t file_position)
{
    size_t index = 0;

    if (file_position.line >= file.lines_num)
        return -1;

    if (file_position.column >= strlen(file.lines[file_position.line]))
        return -1;

    for (size_t i = 0; i < file_position.line; i++)
        index += strlen(file.lines[i]);
    index += file_position.column;

    return index;
}

file_position_t index_to_position(file_content_t file, size_t index)
{
    file_position_t position = {.column = -1, .line = -1};

    if (index > file.characters_num)
        return position;

    size_t current_line_id = 0;
    while (current_line_id < file.lines_num && index >= strlen(file.lines[current_line_id]))
        index -= strlen(file.lines[current_line_id++]);

    if (current_line_id >= file.lines_num)
        return position;

    position.line = current_line_id;
    position.column = index;

    return position;
}

void file_content_clear(file_content_t file)
{
    for (size_t i = 0; i < file.lines_num; i++)
        free(file.lines[i]);
    free(file.lines);
}

// Create lps table for kmp algorithm
size_t *create_lps_table(char *phrase)
{
    size_t *lps = malloc(sizeof(size_t) * strlen(phrase));
    if (!lps)
        ERR("malloc", ALLOCATION_ERROR);

    lps[0] = 0;

    // current longest proper prefix
    size_t clp = 0;
    for (size_t i = 1; i < strlen(phrase); i++)
    {
        if (phrase[i] == phrase[clp])
        {
            clp++;
            lps[i] = clp;
        }
        else
        {
            if (clp > 0)
            {
                clp = lps[clp - 1];
                i--;
            }
            else
                lps[i] = 0;
        }
    }

    return lps;
}

// Return array of starting positions of phrase in file
file_position_t *find_in_file_kmp(file_content_t file, char *phrase, size_t *count)
{
    // Use Knuth-Morris-Pratt algorithm

    size_t *lps = create_lps_table(phrase);
    *count = 0;
    size_t capacity = 1;

    file_position_t *positions = malloc(sizeof(file_position_t) * capacity);
    if (!positions)
        ERR("malloc", ALLOCATION_ERROR);

    size_t i = 0; // index of current element in file content
    size_t j = 0; // index of current element in phrase
    size_t n = file.characters_num;
    size_t m = strlen(phrase);

    while ((n - i) >= (m - j))
    {
        if (phrase[j] == file_content_at(file, i))
        {
            j++;
            i++;
        }

        if (j == m)
        {
            if (*count == capacity)
            {
                capacity *= 2;
                positions = realloc(positions, sizeof(file_position_t) * capacity);
            }
            positions[*count] = index_to_position(file, i - j);
            (*count)++;
            j = lps[j - 1];
        }
        else if (i < n && phrase[j] != file_content_at(file, i))
        {
            if (j != 0)
                j = lps[j - 1];
            else
                i++;
        }
    }

    if ((*count) == 0)
        free(positions);

    free(lps);
    return positions;
}

char *file_content_to_string(file_content_t file)
{
    char *string = malloc(sizeof(char) * (file.characters_num + 1));
    if (!string)
        ERR("malloc", ALLOCATION_ERROR);
    string[0] = '\0';
    for (size_t i = 0; i < file.lines_num; i++)
        strcat(string, file.lines[i]);
    return string;
}
