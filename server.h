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
#define REPORT "report.txt"
#define SUCCESS 0
#define REDIRECT 1
#define C_ERROR 2
#define S_ERROR 3
#define S_M "Success"
#define R_M "Multiple Choices"
#define C_E_M "Bad Request"
#define S_E_M "Internal Server Error"
#define TITLE "EVENT REPORT"
#define T_FONT "Helvetica-Bold"
#define T_SIZE 24
#define SUB_TITLE "Generated on: "
#define ST_FONT "Helvetica"
#define ST_SIZE 18
#define B_FONT "Helvetica"
#define B_SIZE 12
#define TIME_LEN 128
#define BORDER 50
#define ST_SPACE 30
#define NL_SPACE 20

pthread_mutex_t log_mutex; 
pthread_mutex_t queue_mutex;
pthread_mutex_t pdf_mutex;
pthread_cond_t queue_cond;
Queue q_clients;
time_t start_time;
