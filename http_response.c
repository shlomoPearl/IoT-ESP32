#include <stdio.h>
#include "http_response.h"

const char HTTP_VERSION[] = "HTTP/1.1";
int HTTP_CODE[] = {200,300,400,500};
char HTTP_TEXT[][30]  = {"OK","Multiple Choices","Bad Request","Internal Server Error"};

void response_to_s(char* response_s, HTTPResponse* http_res, size_t size){
    const char* delimiter = "\r\n";
    int count = 0;

    // response line
    count += snprintf(response_s, size-count, "%s", http_res->version);
    count += snprintf(response_s + count, size-count," %d",http_res->status_code);
    count += snprintf(response_s + count, size-count, " %s%s",http_res->status_text , delimiter);
    
    // headers
    for (int i=0; i < http_res->header_count; i++){
        count += snprintf(response_s + count, size-count, "%s: %s%s"
            ,http_res->headers[i][K],http_res->headers[i][V], delimiter);
    }
    // blank line
    count += snprintf(response_s + count, size-count, "%s",delimiter);
    // body
    count += snprintf(response_s + count, size-count, "%s%s",http_res->body , delimiter);
}

void build_respone(HTTPResponse* http_res, int code_i, int text_i , const char* body ,size_t body_length){
    snprintf(http_res->version, sizeof(http_res->version), "%s", HTTP_VERSION);
    http_res->status_code = HTTP_CODE[code_i];
    snprintf(http_res->status_text, sizeof(http_res->status_text), "%s", HTTP_TEXT[text_i]);
    http_res->header_count = 0;
    snprintf(http_res->headers[http_res->header_count][K], sizeof(http_res->headers[K]), "Content-Type");
    snprintf(http_res->headers[http_res->header_count][V], sizeof(http_res->headers[V]), "application/json");
    http_res->header_count++;
    snprintf(http_res->headers[http_res->header_count][K], sizeof(http_res->headers[K]), "Content-Length");
    snprintf(http_res->headers[http_res->header_count][V], sizeof(http_res->headers[V]), "%zu", body_length);
    http_res->header_count++;
    snprintf(http_res->body, sizeof(http_res->body), "%s", body);
    http_res->body_length = (int)body_length; 
}