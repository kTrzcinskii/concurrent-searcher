CC=gcc
CFLAGS=-std=gnu17 -Wall -fsanitize=address,undefined
LDFLAGS=-fsanitize=address,undefined
LDLIBS=-lpthread -lm

SRCS=src/concurrent_searcher.c src/entry_list.c src/found_files_list.c src/error_handler.c src/file_content.c
HDRS=src/concurrent_searcher.h src/entry_list.h src/found_files_list.h src/error_handler.h src/file_content.h

TARGET=build/concurrent_searcher

all: ${TARGET}

${TARGET}: ${SRCS} ${HRDS}
	${CC} ${CFLAGS} -o ${TARGET} ${SRCS} ${LDFLAGS} ${LDLIBS}	

.PHONY: clean all

clean:
	-rm -f ${TARGET}