<<<<<<< HEAD
=======
/* Small section of code from HTTrack
 *
 * Authors:
 *      HTTrack Development Team
 *
 * Copyright (C) 1998-2025 HTTrack Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 * 
 * How to run:
 *     gcc htsweb2.c -o htsweb2 -lpthread
 *     ./httrack_bug
 */

>>>>>>> b978fa3 (added data)
 #include <stdio.h>
 #include <pthread.h>
 #include <stdlib.h>
 #include <unistd.h>
 
 typedef struct {
     pthread_mutex_t state;
     int data;
 } Opt;
 
 Opt *global_opt = NULL; // Global variable
 
 void* thread1_func(void* arg) {
    //  sleep(1); // Simulate delay
 
     Opt *opt = (Opt *)malloc(sizeof(Opt));
     pthread_mutex_init(&opt->state, NULL);
     opt->data = 42;
 
     global_opt = opt; // Assigning global variable
 
     return NULL;
 }
 
<<<<<<< HEAD
 void* thread2_func(void* arg) { 
     if (global_opt) {
         pthread_mutex_lock(&global_opt->state);
=======
 void* thread2_func(void* arg) {
     printf("Thread 2: Trying to access global_opt...\n");
 
     if (global_opt) {
         pthread_mutex_lock(&global_opt->state);
         printf("Thread 2: Accessed data = %d\n", global_opt->data);
>>>>>>> b978fa3 (added data)
         pthread_mutex_unlock(&global_opt->state);
     } else {
         printf("Thread 2: global_opt is NULL! Null pointer dereference may occur!\n");
     }
 
     return NULL;
 }
 
<<<<<<< HEAD
=======
//  The buggy interleaving could be:
 
//  Thread 1                      Thread 2
 
//  global_opt = opt             if (global_opt) {
//  = hts_create_opt();            pthread_mutex_lock(&global_opt->state);
//                                  printf("Accessed data = %d", global_opt->data);
//                                }
 
//  In some cases, Thread 2 may execute before Thread 1 assigns the global_opt,
//  resulting in a NULL pointer dereference crash.
>>>>>>> b978fa3 (added data)
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
 