#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

#define NUM_ITERATIONS 500

atomic_int idlers = 0;
pthread_mutex_t idlers_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait_for_idler = PTHREAD_COND_INITIALIZER;

void *listener_thread(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // Delay to allow race with worker
        usleep(rand() % 200);

        if (atomic_load(&idlers) == 0) {
            pthread_mutex_lock(&idlers_mutex);
            if (atomic_load(&idlers) == 0) {
                pthread_cond_wait(&wait_for_idler, &idlers_mutex);
            }
            pthread_mutex_unlock(&idlers_mutex);
        }

        int val = atomic_fetch_sub(&idlers, 1);
        if (val <= 0) {
            fprintf(stderr, "[BUG] idlers underflow detected in listener! (val=%d)\n", val);
            abort();
        }

        // Simulate doing some work
        usleep(rand() % 100);
    }
    return NULL;
}

void *worker_thread(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // Simulate doing work
        usleep(rand() % 150);

        int prev = atomic_fetch_add(&idlers, 1);

        // Delay to simulate race
        usleep(rand() % 100);

        if (prev == 0) {
            pthread_mutex_lock(&idlers_mutex);
            pthread_cond_signal(&wait_for_idler);
            pthread_mutex_unlock(&idlers_mutex);
        }
    }
    return NULL;
}

int main() {
    // add a loop
    for (int i = 0; i < 1000; ++i) {
        srand(time(NULL));
        pthread_t listener, worker;

        pthread_create(&listener, NULL, listener_thread, NULL);
        pthread_create(&worker, NULL, worker_thread, NULL);

        pthread_join(listener, NULL);
        pthread_join(worker, NULL);
    }
   

    printf("Finished without crashing (this is unexpected)\n");
    return 0;
}
