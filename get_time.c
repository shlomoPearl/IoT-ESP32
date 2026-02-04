#include "get_time.h"

void init_time(struct tm* timeinfo) {
    time_t now;
    time(&now);
    localtime_r(&now, timeinfo);
}

void get_current_time_str(char* buffer, size_t size) { 
    time_t now;
    struct tm *local_time;    
    time(&now);
    local_time = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", local_time);
}

void get_time_from_struct(char* buffer, struct tm* timeinfo, size_t size) {
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}