#include "get_time.h"

void get_current_time_str(char* buffer, size_t size) { 
    time_t now;
    struct tm *local_time;    
    time(&now);
    local_time = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", local_time);
}

void get_current_time(time_t* t){
    time(t);
}

bool time_pass(int hours, time_t* start, time_t* end){
    double diff = difftime(*end, *start);
    if (diff >= hours * 3600){
        return true;
    }    
    return false;
}