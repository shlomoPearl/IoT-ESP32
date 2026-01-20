#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

void get_current_time_str(char*, size_t);
void get_current_time(time_t*);
bool time_pass(int, time_t*, time_t*);
