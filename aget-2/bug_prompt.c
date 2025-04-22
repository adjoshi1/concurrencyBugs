#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define NUM_THREADS 2
#define CHUNK_SIZE 100

int bwritten = 0;
pthread_mutex_t bwritten_mutex = PTHREAD_MUTEX_INITIALIZER;
struct thread_data {
    int offset;
};

struct thread_data wthread[NUM_THREADS];

struct Log {
    struct thread_data* wthread;
    int bwritten;
};

struct Log h;

void* http_get(void* arg) {
    int thread_id = *(int*)arg;
    int dw = CHUNK_SIZE;  
    
    wthread[thread_id].offset += dw;

    usleep((rand() % 5 + 1) * 1000); 

    pthread_mutex_lock(&bwritten_mutex);
    bwritten += dw;
    pthread_mutex_unlock(&bwritten_mutex);

    return NULL;
}

void save_log() {
    h.wthread = malloc(sizeof(struct thread_data) * NUM_THREADS);
    memcpy(h.wthread, wthread, sizeof(struct thread_data) * NUM_THREADS);

    usleep((rand() % 5 + 1) * 1000); 

    h.bwritten = bwritten;
}

void simulate_race_condition() {
    bwritten = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        wthread[i].offset = 0;
    }


    pthread_t threads[NUM_THREADS + 1];  
    int thread_ids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, http_get, &thread_ids[i]);
    }


    pthread_create(&threads[NUM_THREADS], NULL, (void*)save_log, NULL);


    for (int i = 0; i < NUM_THREADS + 1; i++) {
        pthread_join(threads[i], NULL);
    }

    if (h.bwritten != bwritten) {
        printf("ERROR: Data race detected! Logged total does not match actual total!\n");
        printf("Actual total: %d, Logged total: %d\n", bwritten, h.bwritten);
    } 

    // Clean up
    free(h.wthread);
}

int main() {
    srand(time(NULL));  

    for (int i = 0; i < 100; i++) {
        simulate_race_condition();
    }

    return 0;
}
