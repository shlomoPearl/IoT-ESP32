#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "cJSON.h"
#include "queue.h"
#include "http_request.h"
#include "http_response.h"
#include "pdf.h"
#include "get_time.h"

#define PORT "7777"  // the port users will be connecting to.
#define MAXDATASIZE 4096
#define MAX_BODY_SIZE 512
#define THREADS_NUM 8
#define BACKLOG 10   // in case the sensor makes multiple recognitions at once.
#define R_NAME "report_"
#define R_TYPE ".pdf"
#define R_SIZE 16
#define SUCCESS 0
#define REDIRECT 1
#define C_ERROR 2
#define S_ERROR 3
#define S_M "Success"
#define R_M "Multiple Choices"
#define C_E_M "Bad Request"
#define S_E_M "Internal Server Error"
#define EVENT_SIZE 128
#define SAVE_INTERVAL_S 60

pthread_mutex_t log_mutex; 
pthread_mutex_t queue_mutex;
pthread_mutex_t report_mutex;
pthread_cond_t queue_cond;
pthread_cond_t report_cond;
Queue q_clients;
time_t start_time;
int report_index = 0;