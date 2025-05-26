#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define LOG_BUFSIZE 1024
#define NUM_THREADS 8
#define ITERS_PER_THREAD 1000
#define STRESS_LOOPS 500

typedef struct {
    char outbuf[LOG_BUFSIZE];
    int outcnt;
} buffered_log;

buffered_log buf;

void* log_writer(void* arg) {
    char msg[64];
    int i;
    for (i = 0; i < ITERS_PER_THREAD; i++) {
        int len = snprintf(msg, sizeof(msg), "Thread %ld log %d\n", (long)arg, i);

        int prev_outcnt = buf.outcnt;
        if (prev_outcnt + len >= LOG_BUFSIZE) {
            buf.outcnt = 0;
            prev_outcnt = 0;
        }

        char* s = &buf.outbuf[prev_outcnt];
        memcpy(s, msg, len);

        if (prev_outcnt != buf.outcnt) {
            printf(">>> Race detected! Thread %ld, prev_outcnt=%d, actual=%d\n",
                   (long)arg, prev_outcnt, buf.outcnt);
            abort();
        }

        buf.outcnt += len;

        usleep(rand() % 50);
    }

    return NULL;
}

int main() {
    srand((unsigned int)time(NULL));

    for (int loop = 0; loop < STRESS_LOOPS; loop++) {
        pthread_t threads[NUM_THREADS];
        buf.outcnt = 0;

        for (long i = 0; i < NUM_THREADS; i++) {
            pthread_create(&threads[i], NULL, log_writer, (void*)i);
        }

        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }

        // Optional: Print final buffer state
        // fwrite(buf.outbuf, 1, buf.outcnt, stdout);
    }

    printf("Stress test complete.\n");
    return 0;
}
