typedef struct {
    char method[16];        // GET, POST, etc.
    char path[256];         // /submit
    char version[16];       // HTTP/1.1
    char headers[20][2][256]; // Array of [key][value]
    int header_count;
    char body[2048];        // Request body
    int body_length;
} HTTPRequest;

HTTPRequest http_parser(char []);