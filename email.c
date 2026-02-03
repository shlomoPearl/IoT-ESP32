#include "email.h"

static const char base64_chars[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define MIME_BOUNDARY "----=_Part_0_123456789"

struct upload_status {
    const char *data;
    size_t bytes_read;
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct upload_status *ctx = userp;
    size_t max = size * nmemb;
    size_t remaining = strlen(ctx->data + ctx->bytes_read);
    if (remaining == 0) return 0;
    
    size_t to_copy = remaining < max ? remaining : max;
    memcpy(ptr, ctx->data + ctx->bytes_read, to_copy);
    ctx->bytes_read += to_copy;
    return to_copy;
}

char *base64_encode(const unsigned char *data, size_t len, size_t *out_len) {
    size_t encoded_len = 4 * ((len + 2) / 3);
    char *out = malloc(encoded_len + 1);
    if (!out) return NULL;
    
    size_t i = 0, j = 0;
    while (i < len) {
        uint32_t a = i < len ? data[i++] : 0;
        uint32_t b = i < len ? data[i++] : 0;
        uint32_t c = i < len ? data[i++] : 0;
        uint32_t triple = (a << 16) | (b << 8) | c;
        
        out[j++] = base64_chars[(triple >> 18) & 0x3F];
        out[j++] = base64_chars[(triple >> 12) & 0x3F];
        out[j++] = base64_chars[(triple >> 6)  & 0x3F];
        out[j++] = base64_chars[triple & 0x3F];
    }
    
    for (size_t k = 0; k < (3 - len % 3) % 3; k++)
        out[encoded_len - 1 - k] = '=';
    
    out[encoded_len] = '\0';
    if (out_len) *out_len = encoded_len;
    return out;
}

char* base64_encode_file(const char *filepath, size_t *out_len) {
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open file: %s\n", filepath);
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    unsigned char *buffer = malloc(file_size);
    if (!buffer) {
        fclose(f);
        fprintf(stderr, "Failed to allocate memory for file\n");
        return NULL;
    }
    
    size_t bytes_read = fread(buffer, 1, file_size, f);
    fclose(f);
    
    if (bytes_read != file_size) {
        free(buffer);
        fprintf(stderr, "Failed to read complete file\n");
        return NULL;
    }
    
    char *encoded = base64_encode(buffer, file_size, out_len);
    free(buffer);
    
    return encoded;
}

static const char* get_filename(const char *path) {
    const char *filename = strrchr(path, '/');
    if (!filename) {
        filename = strrchr(path, '\\');  // Windows support
    }
    return filename ? filename + 1 : path;
}

char* build_email_with_attachment(const Email *email) {
    size_t encoded_len;
    char *pdf_base64 = base64_encode_file(email->pdf_path, &encoded_len);
    if (!pdf_base64) return NULL;
    
    const char *filename = get_filename(email->pdf_path);
    
    // Calculate required size with extra safety margin
    size_t payload_size = 2048 +  // MIME structure
                          strlen(email->from) +
                          strlen(email->to) +
                          strlen(email->subject) +
                          strlen(email->body) +
                          encoded_len;
    
    char *payload = malloc(payload_size);
    if (!payload) {
        free(pdf_base64);
        fprintf(stderr, "Failed to allocate email payload\n");
        return NULL;
    }
    
    snprintf(payload, payload_size,
        "From: <%s>\r\n"
        "To: <%s>\r\n"
        "Subject: %s\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: multipart/mixed; boundary=\"%s\"\r\n"
        "\r\n"
        "--%s\r\n"
        "Content-Type: text/plain; charset=UTF-8\r\n"
        "\r\n"
        "%s\r\n"
        "\r\n"
        "--%s\r\n"
        "Content-Type: application/pdf; name=\"%s\"\r\n"
        "Content-Transfer-Encoding: base64\r\n"
        "Content-Disposition: attachment; filename=\"%s\"\r\n"
        "\r\n"
        "%s\r\n"
        "\r\n"
        "--%s--\r\n",
        email->from, email->to, email->subject, MIME_BOUNDARY,
        MIME_BOUNDARY, email->body,
        MIME_BOUNDARY, filename, filename,
        pdf_base64,
        MIME_BOUNDARY);
    
    free(pdf_base64);
    return payload;
}

static int is_valid_email(const char *email_addr) {
    if (!email_addr || strlen(email_addr) == 0) return 0;
    
    const char *at = strchr(email_addr, '@');
    const char *dot = strrchr(email_addr, '.');
    
    if (!at || !dot) return 0;
    if (at > dot) return 0;
    if (at == email_addr) return 0;
    if (dot == email_addr + strlen(email_addr) - 1) return 0;
    
    return 1;
}

void format_email_body(Email *email) {
    snprintf(email->body, sizeof(email->body),
        "Dear User,\n\n"
        "Your event report has been generated successfully.\n\n"
        "Please find the detailed report attached as a PDF document.\n\n"
        "The attached PDF contains comprehensive information about the\n"
        "reported event, including all relevant data and analysis.\n\n"
        "If you have any questions or need additional information,\n"
        "please feel free to contact our support team.\n\n"
        "Best regards,\n"
        "Automated Event Report System\n\n"
        "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        "This is an automated message. Please do not reply to this email.\n"
        "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
}

void format_email_subject(Email *email) {
    snprintf(email->subject, sizeof(email->subject),
             "# Event Report #");
}

int send_email(const Email *email) {
    // Validate inputs
    if (!email) {
        fprintf(stderr, "Email structure is NULL\n");
        return -1;
    }
    
    if (!is_valid_email(email->from)) {
        fprintf(stderr, "Invalid sender email: %s\n", email->from);
        return -1;
    }
    
    if (!is_valid_email(email->to)) {
        fprintf(stderr, "Invalid recipient email: %s\n", email->to);
        return -1;
    }
    
    if (strlen(email->pdf_path) == 0) {
        fprintf(stderr, "PDF path is empty\n");
        return -1;
    }
    
    // Verify PDF exists
    FILE *test = fopen(email->pdf_path, "rb");
    if (!test) {
        fprintf(stderr, "PDF file not found: %s\n", email->pdf_path);
        return -1;
    }
    fclose(test);
    
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return -1;
    }
    
    char *payload = build_email_with_attachment(email);
    if (!payload) {
        curl_easy_cleanup(curl);
        return -1;
    }
    
    struct upload_status upload_ctx = { payload, 0 };
    char* mail_s = getenv("MAIL_S");
    char* mail_pass = getenv("MAIL_PASS");
    if (!mail_s || !mail_pass) {
        fprintf(stderr, "Missing MAIL_S or MAIL_PASS environment variables\n");
        fprintf(stderr, "Please set: export MAIL_S=your@email.com\n");
        fprintf(stderr, "           export MAIL_PASS=your_app_password\n");
        free(payload);
        curl_easy_cleanup(curl);
        return -1;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, SMTP_SERVER);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_USERNAME, mail_s);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, mail_pass);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, email->from);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    
    struct curl_slist *recipients = NULL;
    recipients = curl_slist_append(recipients, email->to);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
  
#ifdef DEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
    
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "Email send failed: %s\n", curl_easy_strerror(res));
    } else {
        printf("Email sent successfully to %s\n", email->to);
    }
    
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    free(payload);
    return (res == CURLE_OK) ? 0 : -1;
}