#pragma once
#include <stdlib.h>
#define K 0
#define V 1

typedef struct {
    char version[16];       
    int status_code;        
    char status_text[16];        
    char headers[20][2][256]; // Array of [key][value]
    int header_count;
    char body[2048];        
    int body_length;
} HTTPResponse;

void response_to_s(char* ,HTTPResponse, size_t); 
void build_respone(HTTPResponse*, int, int , const char* ,size_t);