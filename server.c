#include "server.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void log_event(PDF* pdf, char event[]) {
    pthread_mutex_lock(&log_mutex);
    write_to_pdf(pdf, event, pdf->border);
    pdf->y_pos -= NL_SPACE;
    pthread_mutex_unlock(&log_mutex);
}

void json_to_str(char event_buffer[], cJSON *json) {
    char* event_str = NULL;
    char* time_e_str = NULL;
    char* email_str = NULL;
    cJSON *event = cJSON_GetObjectItemCaseSensitive(json, "EVENT");
    cJSON *time_e = cJSON_GetObjectItemCaseSensitive(json, "TIME");
    cJSON *email = cJSON_GetObjectItemCaseSensitive(json, "EMAIL");
    if ((cJSON_IsString(event) && (event->valuestring != NULL))
    && (cJSON_IsString(time_e) && (time_e->valuestring != NULL))
    && (cJSON_IsString(email) && (email->valuestring != NULL))) {
        event_str = event->valuestring;
        time_e_str = time_e->valuestring;
        email_str = email->valuestring;
    }
    if (event_str != NULL && time_e_str != NULL && email_str != NULL) {
        strncpy(to_mail, email_str, sizeof(to_mail) - 1);
        to_mail[sizeof(to_mail) - 1] = '\0';
        snprintf(event_buffer, EVENT_SIZE, "EVENT: %s | TIME: %s",
                                           event_str, time_e_str);        
    }
}

void* report_writer(void* arg){
    PDF* pdf_struct = (PDF*)arg;
    if (pdf_struct == NULL) {
        perror("PDF struct is NULL");
        return NULL;
    }
    while (1) {
        struct timespec ts;
        ts.tv_sec = time(NULL) + SAVE_INTERVAL_S;
        ts.tv_nsec = 0;
        pthread_mutex_lock(&report_mutex);
        int result = pthread_cond_timedwait(&report_cond, &report_mutex, &ts);
        if (result == ETIMEDOUT) {
            char report_name[16];
            snprintf(report_name, R_SIZE, "%s%d%s", R_NAME, report_index++, R_TYPE);
            save_pdf(pdf_struct, report_name);
            free_pdf(pdf_struct);
            get_current_time(&start_time);
            init_pdf(pdf_struct);
            printf("Report saved: %s\n", report_name);
            Email email = {0};
            strncpy(email.from, MAIL_SENDER, sizeof(email.from) - 1);
            email.from[sizeof(email.from) - 1] = '\0';
            strncpy(email.to, to_mail, sizeof(email.to) - 1);
            email.to[sizeof(email.to) - 1] = '\0';
            format_email_subject(&email);
            format_email_body(&email);
            strncpy(email.pdf_path, report_name, sizeof(email.pdf_path) - 1);
            email.pdf_path[sizeof(email.pdf_path) - 1] = '\0';
        }
        pthread_mutex_unlock(&report_mutex);
    }
    return NULL;
}

void* data_handler(void* arg) {
    PDF* pdf_struct = (PDF*)arg;
    while (1) {
        pthread_mutex_lock(&queue_mutex);
        while (isEmpty(&q_clients)) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        } 
        int new_fd = dequeue(&q_clients);
        pthread_mutex_unlock(&queue_mutex);
        
        int numbytes;
        char buf[MAXDATASIZE];
        char res_str[MAXDATASIZE];
        HTTPResponse http_response;
        char response_body[MAX_BODY_SIZE];

        if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            snprintf(response_body, MAX_BODY_SIZE, "{\"message\":\"%s\"}", S_E_M);
            build_respone(&http_response, S_ERROR, S_ERROR, response_body, MAXDATASIZE);
            response_to_s(res_str, &http_response, MAXDATASIZE);
            send(new_fd, res_str, strlen(res_str), 0);
            close(new_fd);
            continue;
        }
        if (numbytes == 0) {
            // Connection closed by client
            close(new_fd);
            continue;
        }
        buf[numbytes] = '\0';
        HTTPRequest http_request = http_parser(buf);
        if (strcmp(http_request.method, "POST") == 0 && http_request.body_length > 0){
            cJSON *json = cJSON_Parse(http_request.body);
            if (json == NULL){
                snprintf(response_body, MAX_BODY_SIZE, "{\"message\":\"%s\"}", C_E_M);
                build_respone(&http_response, C_ERROR, C_ERROR, response_body, MAXDATASIZE);
                response_to_s(res_str, &http_response, MAXDATASIZE);
                send(new_fd, res_str, strlen(res_str), 0);
                close(new_fd);
                continue;
            } else {
                char event_buffer[EVENT_SIZE];
                json_to_str(event_buffer, json);
                log_event(pdf_struct, event_buffer);
            }
            cJSON_Delete(json);
        }
        snprintf(response_body, MAX_BODY_SIZE, "{\"message\":\"%s\"}", S_M);
        build_respone(&http_response, SUCCESS, SUCCESS, response_body, MAX_BODY_SIZE);
        response_to_s(res_str, &http_response, MAXDATASIZE);
        if ((numbytes = send(new_fd, res_str, strlen(res_str), 0)) != (int)strlen(res_str)){
            perror("response");
        }
        close(new_fd);
        continue;
    }
    return NULL;
}

int main(void) {
    // listen on sock_fd, new connection on new_fd
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address info
    socklen_t sin_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    pthread_t th[THREADS_NUM];
    pthread_mutex_init(&log_mutex, NULL);
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&report_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);
    pthread_cond_init(&report_cond, NULL);
    initializeQueue(&q_clients);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        freeaddrinfo(servinfo);
        return 1;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
                perror("server: socket");
                continue;
        }
            
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
                    perror("setsockopt");
                    exit(1);
        }
                
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                    close(sockfd);
                    perror("server: bind");
                    continue;
        }            
                break;
    }
            
    freeaddrinfo(servinfo); // all done with this structure
            
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    
    printf("server: waiting for connections...\n");
    get_current_time(&start_time);
    PDF* pdf_struct = malloc(sizeof(PDF));
    init_pdf(pdf_struct);

    for (int i = 0; i < THREADS_NUM - 1; i++) {
        if (pthread_create(&th[i], NULL, data_handler, (void*)pdf_struct) != 0) {
            perror("pthread_create");
        }
    }
    pthread_create(&th[THREADS_NUM - 1], NULL, report_writer, (void*)pdf_struct);

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
            &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;   
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        pthread_mutex_lock(&queue_mutex);
        enqueue(&q_clients, new_fd);
        pthread_mutex_unlock(&queue_mutex);
        pthread_cond_broadcast(&queue_cond);
    }
    for (int i = 0; i < THREADS_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }
    free(pdf_struct);
    pthread_mutex_destroy(&log_mutex);
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&report_mutex);
    pthread_cond_destroy(&queue_cond);
    pthread_cond_destroy(&report_cond);
    close(sockfd);
    return 0;
}
