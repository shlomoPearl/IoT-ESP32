FLAGGS = -Wall -Wextra
CC = gcc
all: server
server: cJSON.c cJSON.h queue.c queue.h server.c http_request.h http_request.c 
	$(CC) -o server server.c queue.c cJSON.c http_request.c -pthread $(FLAGGS)

run:
	./server

.PHONY: all run clean

clean:
	rm -f server report.txt