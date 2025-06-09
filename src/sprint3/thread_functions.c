#include <stddef.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "structs.h"

void* collision_detection_func(void *arg) {

    write(STDOUT_FILENO, "Collision Thread\n", 18);

    collition_thread_info info = *((collition_thread_info*) arg);

    int total_time_steps = info.time_step_num;
    int total_drones = info.drone_num;
    Position *matrix = info.matrix;

    for (int t = 0; t < total_time_steps; t++) {

        for (int i = 0; i < total_drones; i++) {

            Position pos_i = matrix[POS_IDX(t, i, total_drones)];

            for (int j = i + 1; j < total_drones; j++) {

                Position pos_j = matrix[POS_IDX(t, j, total_drones)];

                if (pos_i.x == pos_j.x && pos_i.y == pos_j.y && pos_i.z == pos_j.z) {

                    // TODO : Collition occurred: notify report thread
                }

            }
        }
    }
    pthread_exit(NULL);
}

void* report_func(void *arg) {

    write(STDOUT_FILENO, "Report Thread\n", 14);

    pthread_exit(NULL);
}

void create_threads(pthread_t thread_ids[], int drone_num, int time_step_num, Position *matrix) {

    collition_thread_info info;
    info.drone_num = drone_num;
    info.time_step_num = time_step_num;
    info.matrix = matrix;

    if ((pthread_create(&thread_ids[0], NULL, collision_detection_func, (void*) &info) != 0)) {
        perror("pthread_create failed");
        exit(2);
    }

    if ((pthread_create(&thread_ids[1], NULL, report_func, NULL) != 0)) {
        perror("pthread_create failed");
        exit(3);
    }
}
