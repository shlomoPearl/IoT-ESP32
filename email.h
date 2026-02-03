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

char* base64_encode(const unsigned char *data, size_t len, size_t *out_len);
char* base64_encode_file(const char *filepath, size_t *out_len);
char* build_email_with_attachment(const Email *email);
int send_email(const Email *email);

void format_email_body(Email *email);
void format_email_subject(Email *email);