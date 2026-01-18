#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "http_request.h"

#define K 0
#define V 1


HTTPRequest http_parser(char request_s[]) {
    HTTPRequest http_request;
    http_request.body_length = 0;
    int body_f = 0;
    http_request.header_count = 0;
    const char *delimiter = "\r\n";
    const char *body_delimiter = "\r\n\r\n";
    char* body = strstr(request_s, body_delimiter);
    *body = '\0';
    body += strlen(body_delimiter);
    strncpy(http_request.body, body, sizeof(http_request.body));
    char* line = strtok(request_s, delimiter);
    sscanf(line, "%s %s %s", http_request.method, http_request.path, http_request.version);
    while (line != NULL) {
        if (body_f){
            strcpy(http_request.body, line);
        } else{
            char key[256], value[256];
            sscanf(line, "%[^:]: %[^\r\n]", key, value);
            if (strcmp(key, "Content-Length") == 0) {
                http_request.body_length = atoi(value);
                body_f = 1;
            }
            strcpy(http_request.headers[http_request.header_count][K], key);
            strcpy(http_request.headers[http_request.header_count][V], value);
            http_request.header_count++;
        }
        line = strtok(NULL, delimiter);
    }
    return http_request;
}