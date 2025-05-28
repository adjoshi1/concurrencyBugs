#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

typedef struct {
    int dummy;
} Bandwidth;

typedef struct {
    Bandwidth* bandwidth;
} Handle;

Handle* h;
int bug_count = 0;
pthread_mutex_t bug_mutex = PTHREAD_MUTEX_INITIALIZER;

Bandwidth* tr_bandwidthNew(Handle* h, void* unused) {
    Bandwidth* bw = malloc(sizeof(Bandwidth));
    bw->dummy = 42;
    printf("[Init] Bandwidth initialized\n");
    return bw;
}

int tr_isBandwidth(Bandwidth* b) {
    return b != NULL && b->dummy == 42;
}

void* allocateBandwidth(void* arg) {
    usleep(rand() % 1000);  
    if (!tr_isBandwidth(h->bandwidth)) {
        pthread_mutex_lock(&bug_mutex);
        bug_count++;
        fprintf(stderr, "[Bug %d] Bandwidth not initialized!\n", bug_count);
        pthread_mutex_unlock(&bug_mutex);
        abort();
    } else {
        printf("[OK] Bandwidth valid\n");
    }
    return NULL;
}

void* sessionInit(void* arg) {
    usleep(rand() % 1000);  
    h->bandwidth = tr_bandwidthNew(h, NULL);
    return NULL;
}

int main() {
    srand(time(NULL));
    h = malloc(sizeof(Handle));

    const int iterations = 500;

    for (int i = 0; i < iterations; ++i) {
        pthread_t t1, t2;
        h->bandwidth = NULL;

        pthread_create(&t1, NULL, sessionInit, NULL);
        pthread_create(&t2, NULL, allocateBandwidth, NULL);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
    }

    printf("\nTest completed. Total bugs detected: %d out of %d iterations.\n", bug_count, iterations);

    free(h->bandwidth);
    free(h);
    return 0;
}
