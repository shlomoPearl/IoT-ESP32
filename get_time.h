#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

void init_time(struct tm*);
void get_current_time_str(char*, size_t);
void get_time_from_struct(char*, struct tm*, size_t);