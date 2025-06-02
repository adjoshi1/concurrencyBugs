#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define LOG_OPEN 1
#define LOG_CLOSED 0

volatile int log_type = LOG_OPEN;

void* thread_new_file(void* arg) {
    // Simulate closing the log
    printf("[Thread 1] Closing log...\n");
    log_type = LOG_CLOSED; // Partial state
    usleep(100000);        // Delay to allow thread 2 to run
    printf("[Thread 1] Reopening log...\n");
    log_type = LOG_OPEN;   // Restoring full state
    return NULL;
}

void* thread_log_insert(void* arg) {
    usleep(50000); // Run in the middle of thread 1
    int local_log_type = log_type;

    if (local_log_type != LOG_CLOSED) {
        printf("[Thread 2] Logging event (log_type = %d)\n", local_log_type);
    } else {
        printf("[BUG] Skipped logging! (log_type = %d)\n", local_log_type);
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
