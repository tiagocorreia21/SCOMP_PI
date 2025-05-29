#include <stddef.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void* collision_detection_func(void *arg) {

    write(STDOUT_FILENO, "Collision Thread\n", 18);

    pthread_exit(NULL);
}

void* report_func(void *arg) {

    write(STDOUT_FILENO, "Report Thread\n", 14);

    pthread_exit(NULL);
}

void create_threads(pthread_t thread_ids[]) {

    if ((pthread_create(&thread_ids[0], NULL, collision_detection_func, NULL) != 0)) {
        perror("pthread_create failed");
        exit(2);
    }

    if ((pthread_create(&thread_ids[1], NULL, report_func, NULL) != 0)) {
        perror("pthread_create failed");
        exit(3);
    }
}
