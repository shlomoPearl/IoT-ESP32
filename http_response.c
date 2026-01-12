#include <stdio.h>
#include "http_response.h"

#define K 0
#define V 1

void response_to_s(char* response_s, HTTPResponse http_res, size_t size){
    const char* delimiter = "\r\n";
    int count = 0;

    // response line
    count += snprintf(response_s, size-count, "%s", http_res.version);
    count += snprintf(response_s + count, size-count," %d",http_res.status_code);
    count += snprintf(response_s + count, size-count, " %s%s",http_res.status_text , delimiter);
    
    // headers
    for (int i=0; i < http_res.header_count; i++){
        count += snprintf(response_s + count, size-count, "%s: %s%s"
            ,http_res.headers[i][K],http_res.headers[i][V], delimiter);
    }
    // blank line
    count += snprintf(response_s + count, size-count, "%s",delimiter);
    // body
    count += snprintf(response_s + count, size-count, "%s%s",http_res.body , delimiter);
}