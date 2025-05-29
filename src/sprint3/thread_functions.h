#ifndef THREAD_FUNCTIONS_H
#define THREAD_FUNCTIONS_H

void* collision_detection_func(void *arg);
void* report_func(void *arg);
void create_threads(pthread_t thread_ids[]);

#endif