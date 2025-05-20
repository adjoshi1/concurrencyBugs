#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    pthread_mutex_t state;
    int data;
} Opt;

Opt *global_opt = NULL; 

void* thread1_func(void* arg) { 
    Opt *opt = (Opt *)malloc(sizeof(Opt));
    pthread_mutex_init(&opt->state, NULL);
    opt->data = 42;

    global_opt = opt; 

    return NULL;
}

void* thread2_func(void* arg) { 
    if (global_opt) {
        pthread_mutex_lock(&global_opt->state);
        pthread_mutex_unlock(&global_opt->state);
    } else {
        printf("Thread 2: global_opt is NULL! Null pointer dereference may occur!\n");
    }

    return NULL;
}

int main() {
    pthread_t t1, t2;

    for (int i = 0; i < 1000; i++) {
        global_opt = NULL;

        pthread_create(&t1, NULL, thread1_func, NULL);
        pthread_create(&t2, NULL, thread2_func, NULL);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

        if (global_opt) {
            pthread_mutex_destroy(&global_opt->state);
            free(global_opt);
        }
    }

    return 0;
}

