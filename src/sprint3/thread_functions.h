#ifndef THREAD_FUNCTIONS_H
#define THREAD_FUNCTIONS_H

#include "structs.h"

void* collision_detection_func(void *arg);

void* report_func(void *arg);

void create_threads(pthread_t thread_ids[], int drone_num, int time_step_num, Position *matrix);

void simulation_finished(void);

#endif