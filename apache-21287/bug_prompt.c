#include <time.h>  // Required for nanosleep()
#include <pthread.h>
#include <stdatomic.h>

typedef struct {
    atomic_int refcount;
    int cleanup;
    int cleanup_count;
} CacheObject;

void cleanup_cache_object(CacheObject *obj) {
    struct timespec ts = {0, 100000000};  
    nanosleep(&ts, NULL);
    obj->cleanup_count++;
    if (obj->cleanup_count > 1) {
        printf("ERROR: Cache object has been cleaned up twice!\n");
    }
}

void *decrement_refcount(void *arg) {
    CacheObject *obj = (CacheObject *)arg;

  
    atomic_fetch_sub(&obj->refcount, 1);

   
    struct timespec ts = {0, 5000000}; 
    nanosleep(&ts, NULL);

    if (atomic_load(&obj->refcount) == 0) {
        if (obj->cleanup) {
            cleanup_cache_object(obj);
        }
    }
    return NULL;
}

void run_bug() {
    for (int i = 0; i < 100; i++) { 
        CacheObject *cache_obj = malloc(sizeof(CacheObject));
        cache_obj->refcount = 2;
        cache_obj->cleanup = 1;
        cache_obj->cleanup_count = 0;

        pthread_t t1, t2;
        pthread_create(&t1, NULL, decrement_refcount, cache_obj);
        pthread_create(&t2, NULL, decrement_refcount, cache_obj);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

        free(cache_obj);
    }
}

int main() {
    run_bug();
    return 0;
}

