# Concurrent Searcher

## Introduction

Concurrent Searcher is command-line tool written in C that enables users to search for specific phrase within files inside chosen directories. It uses phtreads to make searching faster by multi-threading. 

Main goal of this project was to practice multi-threading as well as generally writting C POSIX programs.

## Installation and compilation
This program will only work on POSIX-complaint OS.
For installing program you have to git clone this repository. Then you just have to run:
```shell
make all
```
inside project directory (note that you will need to have `gcc` and `make` installed on your machine). 
Compiled file is waiting for being used at `./build/concurrent_searcher`.

## Usage

```shell
./build/concurrent_searhcer [options] <...directories>  
```

<...directories> are paths (separated by `<space>`) to directories in which files should be checked.

#### Available options:

| Name             | Symbol | Description                                                                                                                | Takes argument | Argument type        | Optional |
|------------------|--------|----------------------------------------------------------------------------------------------------------------------------|----------------|----------------------|----------|
| Phrase           | -p     | Phrase to be looked for inside files in provided directories.                                                              | Yes            | string               | No       |
| Recursively      | -r     | If passed then all folders are checked recursively.                                                                        | No             | -                    | Yes      |
| Input path       | -i     | Path to file, which content should be treated as `Phrase` parameter.                                                       | Yes            | string               | Yes      |
| Output path      | -o     | Path to file, in which program output should be stored. If no path is provided then output is returned using stdout.       | Yes            | string               | Yes      |
| Directories path | -d     | Path to file, which every line should be added as new entry in `Directories`.                                              | Yes            | string               | Yes      |
| Threads number   | -t     | Number of threads to be initialized. Default value is minimum of max range number (16) and number of provided directories. | Yes            | number in range 1-16 | Yes      |

**Important note**: always one of parameters `Phrase` and `Input path` must be passed. If both are passed then only `Phrase` is used.

 - Basic example - searching for "i love/hate c" in every file in directory `./very_cool_directory` recursively:
    ```shell
    ./build/concurrent_searcher -r -p "i love/hate c" ./very_cool_directory
    ```

 - Advanced example - search for phrase stored in file `./input.txt` in every directory listed in `./my_dirs` + in directory `/some/other/directory` using `12 threads` saving output to `./output.txt`, all of that recursively:
    ```shell
    ./build/concurrent_searcher -r -i ./input.txt -o ./output.txt -t 12 -d ./my_dirs /some/other/directory
    ```

#### Output
Output is in form as follows:
```
<number of found entries (let's say n)>
<entry_1>
<entry_2>
...
<entry_n>
```

where each `entry` is of form:
```
<file_path> <start>-<end>    
```

`start` and `end` are line and column of respectively, starting and ending position, in form of:
```
<line>:<column>
```

## Implementation
- To store directories and files in which `Phrase` was found I used the very basic type of linked-list (`directories_list_t` and `found_files_list_t`).
- To search for phrase in files I used Knuth-Morris-Pratt algorithm.
- To implement multi-threading I used `pthread` library.

## References
- [Wikipedia - Knuth–Morris–Pratt algorithm](https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm)
- [AbdulBari - 9.1 Knuth-Morris-Pratt KMP String Matching Algorithm](https://www.youtube.com/watch?v=V5-7GzOfADQ&ab_channel=AbdulBari)
- [GeeksForGeeks - KMP Algorithm for Pattern Searching](https://www.geeksforgeeks.org/kmp-algorithm-for-pattern-searching/)