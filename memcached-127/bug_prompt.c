#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define INCR_MAX_STORAGE_LEN 32

// Simulated item structure
typedef struct {
    char data[INCR_MAX_STORAGE_LEN];
    size_t nbytes;
    int use_cas;
    unsigned long long cas;
} item;

item *shared_item;

void ITEM_set_cas(item *it, unsigned long long cas) {
    it->cas = cas;
}

unsigned long long get_cas_id() {
    static unsigned long long cas_id = 0;
    return ++cas_id;
}

item* do_item_alloc(size_t nbytes) {
    item *new_it = malloc(sizeof(item));
    if (!new_it) return NULL;
    memset(new_it->data, 0, sizeof(new_it->data));
    new_it->nbytes = nbytes;
    return new_it;
}

void item_replace(item *old_it, item *new_it) {
    usleep(rand() % 100); 
    shared_item = new_it;
    free(old_it);
}

void do_item_remove(item *it) {
}

int do_add_delta(item *it, int incr, int64_t delta) {
    usleep(rand() % 100); 
    unsigned long long old_value = strtoull(it->data, NULL, 10);
    unsigned long long value = incr ? old_value + delta : old_value - delta;

    char buf[INCR_MAX_STORAGE_LEN];
    int res = snprintf(buf, INCR_MAX_STORAGE_LEN, "%llu", value);

    if (res + 2 > it->nbytes) {
        item *new_it = do_item_alloc(res + 2);
        if (!new_it) return -1;
        memcpy(new_it->data, buf, res);
        memcpy(new_it->data + res, "\r\n", 2);
        item_replace(it, new_it);
    } else {
        ITEM_set_cas(it, get_cas_id());
        memcpy(it->data, buf, res);
        memset(it->data + res, ' ', it->nbytes - res - 2);
    }

    usleep(rand() % 100);
    unsigned long long observed = strtoull(shared_item->data, NULL, 10);
    if (observed < old_value) {
        fprintf(stderr, "ERROR: Atomicity violation detected: value reverted from %llu to %llu\n", old_value, observed);
        abort();
    }

    return 0;
}

void add_delta(item *it, int incr, int64_t delta) {
    do_add_delta(it, incr, delta);
}

void* thread_func(void* arg) {
    for (int i = 0; i < 1000; ++i) {
        item *local = shared_item;
        add_delta(local, 1, 1);
        usleep(rand() % 100);
    }
    return NULL;
}

int main() {
    for (int run = 0; run < 500; ++run) {
        shared_item = do_item_alloc(8);  
        strcpy(shared_item->data, "0\r\n");

        pthread_t t1, t2;
        pthread_create(&t1, NULL, thread_func, NULL);
        pthread_create(&t2, NULL, thread_func, NULL);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

        unsigned long long final = strtoull(shared_item->data, NULL, 10);
        unsigned long long expected = 2 * 1000;

        if (final != expected) {
            fprintf(stderr, "ERROR: Final value mismatch: expected %llu, got %llu\n", expected, final);
            abort();
        }

        free(shared_item);
    }
    return 0;
}