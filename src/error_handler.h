#ifndef ERROR_HANDLER
#define ERROR_HANDLER

#define GENERAL_ERROR EXIT_FAILURE
#define ALLOCATION_ERROR 2

#define NOT_IMPLEMENTED 100

#define ERR(source, error_code) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(error_code))

#endif // ERROR_HANLDER