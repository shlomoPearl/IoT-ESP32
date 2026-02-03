FLAGGS = -Wall -Wextra
CC = gcc
H = http
all: server
server: cJSON.c cJSON.h queue.c queue.h server.c $(H)_request.h $(H)_request.c $(H)_response.h $(H)_response.c pdf.h pdf.c get_time.h get_time.c email.h email.c
	$(CC) -o server server.c queue.c cJSON.c http_request.c http_response.c pdf.c get_time.c email.c -lpthread -lhpdf -lcurl $(FLAGGS)

run:
	./server

.PHONY: all run clean

clean:
	rm -f server report_*.pdf