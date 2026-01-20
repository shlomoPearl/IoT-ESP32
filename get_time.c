#include "get_time.h"

char* get_current_time_str(char* buffer, size_t size) { 
    time_t now;
    struct tm *local_time;    
    time(&now);
    local_time = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", local_time);
    return buffer;
}

