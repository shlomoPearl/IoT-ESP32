FLAGGS = -Wall -Wextra
CC = gcc
all: server
server: cJSON.c cJSON.h queue.c queue.h server.c 
	$(CC) -o server server.c queue.c cJSON.c -pthread $(FLAGGS)

run:
	./server

.PHONY: all clean

clean:
	rm -f server