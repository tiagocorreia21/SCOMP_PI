#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "structs.h"
#include "functions.h"
#include <sys/mman.h>
#include "thread_functions.h"
#include <pthread.h>

#define TIME_STEPS_NUM 4
#define MAX_COLLISION_NUM 2
#define DRONE_NUM 2
#define THREAD_NUM 2

int main() {

	Position* matrix = allocate_shared_position_matrix(DRONE_NUM, TIME_STEPS_NUM, "/matrix_shared_data");

	if (matrix == NULL) {
	    perror("matrix allocation failed");
        exit(10);
    }

    pthread_t thread_ids[THREAD_NUM];

    create_threads(thread_ids);

	int p[DRONE_NUM]; //PIDs

	for (int i = 0; i < DRONE_NUM; i++) {

		p[i] = fork();

		if (p[i] == -1) {
			perror("Fork Failed");
			exit(11);
		}

		if (p[i] == 0) {

			// child/drone process
            /*for (int j = 0; j < TIME_STEPS_NUM; j++) {
				run_drone_script(j, matrix, i, TIME_STEPS_NUM);
            }*/
            exit(0);
		}
	}

	//=================================================================================================================

	//Parent Process

    // wait for all the child process to finish
    printf("Waiting for drone processes to finish...\n\n");
    for (int i = 0; i < DRONE_NUM; i++) {

     	int status;

       	int finished_pid = waitpid(p[i], &status, 0);

       	if (WIFEXITED(status)) {
       		printf("Drone %d with PID %d ended with value %d\n", i, finished_pid, WEXITSTATUS(status));
       	}
    }
    printf("============================================\n\n");

    //wait for all threads to finish
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    //free_position_matrix(matrix, "/matrix_shared_data");

	printf("Simulation finished with success...\n");

	return 0;
}

