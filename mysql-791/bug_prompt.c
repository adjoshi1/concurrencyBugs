#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define LOG_OPEN 1
#define LOG_CLOSED 0

volatile int log_type = LOG_OPEN;

void* thread_new_file(void* arg) {
    log_type = LOG_CLOSED; 
    usleep(100000);       
    log_type = LOG_OPEN;  
    return NULL;
}

void* thread_log_insert(void* arg) {
    usleep(50000); 
    int local_log_type = log_type;

    if (local_log_type != LOG_CLOSED) {
       
    } else {
        printf("ERROR: Skipped logging! (log_type = %d)\n", local_log_type);
    }

    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread_new_file, NULL);
    pthread_create(&t2, NULL, thread_log_insert, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
