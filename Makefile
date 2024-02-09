CC=gcc
CFLAGS=-std=gnu17 -Wall -fsanitize=address,undefined
LDFLAGS=-fsanitize=address,undefined
LDLIBS=-lpthread -lm

SRCS=src/concurrent_searcher.c
HDRS=src/concurrent_searcher.h

TARGET=build/concurrent_searcher

all: ${TARGET}

${TARGET}: ${SRCS} ${HRDS}
	${CC} ${CFLAGS} -o ${TARGET} ${SRCS} ${LDFLAGS} ${LDLIBS}	

.PHONY: clean all

clean:
	-rm -f ${TARGET}