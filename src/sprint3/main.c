#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "structs.h"
#include "functions.h"
#include <sys/mman.h>

#define TIME_STEPS_NUM 4
#define MAX_COLLISION_NUM 2
#define DRONE_NUM 10

int main() {

	Position*** positions_matrix = allocate_position_matrix(DRONE_NUM, TIME_STEPS_NUM);

	if (positions_matrix == NULL) {
        return EXIT_FAILURE;
    }

	int p[DRONE_NUM]; //PIDs

	for (int i = 0; i < DRONE_NUM; i++) {

		p[i] = fork();

		if (p[i] == -1) {
			perror("Fork Failed");
			exit(1);
		}

		if (p[i] == 0) {

			// child/drone process

            for (int j = 0; j < TIME_STEPS_NUM; j++) {
				run_drone_script(j, positions_matrix, i, TIME_STEPS_NUM);
            }

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

	printf("Simulation finished with success...\n");

	return 0;
}

