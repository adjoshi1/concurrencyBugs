#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct sp_head {
    char name[32];
    int valid;
} sp_head;

typedef struct sp_cache {
    sp_head* proc;
    unsigned long version;
} sp_cache;

sp_cache global_cache = { NULL, 0 };
pthread_mutex_t Cversion_lock = PTHREAD_MUTEX_INITIALIZER;
unsigned long Cversion = 1;

sp_head* sp_head_create(const char* name) {
    sp_head* p = malloc(sizeof(sp_head));
    strcpy(p->name, name);
    p->valid = 1;
    return p;
}

void sp_head_execute(sp_head* p) {
    
    usleep(100000);  

    if (!p->valid) {
        fprintf(stderr, "ERROR: Procedure '%s' invalidated during execution!\n", p->name);
        exit(1);
    }
}

void sp_cache_remove_all(sp_cache* cache) {
    if (cache->proc) {
        cache->proc->valid = 0;
        cache->proc = NULL;
    }
}

sp_head* sp_cache_lookup(sp_cache* cache, const char* name, unsigned long global_version) {
    if (cache->version < global_version) {
        sp_cache_remove_all(cache);
        cache->version = global_version;
        return NULL;
    }
    if (cache->proc && strcmp(cache->proc->name, name) == 0)
        return cache->proc;
    return NULL;
}

void sp_cache_insert(sp_cache* cache, sp_head* p, unsigned long global_version) {
    if (cache->version < global_version) {
        sp_cache_remove_all(cache);
        cache->version = global_version;
    }
    cache->proc = p;
}

void sp_cache_remove(sp_cache* cache, const char* name, unsigned long new_version) {
    if (cache->proc && strcmp(cache->proc->name, name) == 0) {
        cache->proc->valid = 0;
        cache->proc = NULL;
    }
    cache->version = new_version + 1;
}

sp_head* sp_find_procedure(const char* name) {
    unsigned long v;
    pthread_mutex_lock(&Cversion_lock);
    v = Cversion;
    pthread_mutex_unlock(&Cversion_lock);

    sp_head* p = sp_cache_lookup(&global_cache, name, v);
    if (!p) {
        p = sp_head_create(name);
        sp_cache_insert(&global_cache, p, v);
    }
    return p;
}

void* thread_call_proc(void* arg) {
    sp_head* proc = sp_find_procedure("foo");

    usleep(50000); 
    if (proc)
        sp_head_execute(proc);
    return NULL;
}

void* thread_drop_proc(void* arg) {
    usleep(60000); 

    pthread_mutex_lock(&Cversion_lock);
    unsigned long v = ++Cversion;
    pthread_mutex_unlock(&Cversion_lock);

    sp_cache_remove(&global_cache, "foo", v);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_call_proc, NULL);
    pthread_create(&t2, NULL, thread_drop_proc, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
