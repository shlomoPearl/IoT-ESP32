#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define SMTP_SERVER "smtp://smtp.gmail.com:587"

typedef struct Email {
    char to[32];
    char from[32];
    char subject[256];
    char body[2048];
    char pdf_path[256];
} Email;

char* base64_encode(const unsigned char *, size_t , size_t *);
char* base64_encode_file(const char*, size_t*);
char* build_email_with_attachment(const Email*);
int send_email(const Email *email);

void format_email_body(Email*, int, const char*, const char*);
void format_email_subject(Email*, int);