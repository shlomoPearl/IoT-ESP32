FLAGGS = -Wall -Wextra
CC = gcc
H = http
all: server
server: cJSON.c cJSON.h queue.c queue.h server.c $(H)_request.h $(H)_request.c $(H)_response.h $(H)_response.c 
	$(CC) -o server server.c queue.c cJSON.c http_request.c http_response.c -pthread $(FLAGGS)

run:
	./server

.PHONY: all run clean

clean:
	rm -f server report.txt