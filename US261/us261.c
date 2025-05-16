#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "structs.h"
#include "functions.h"
#include <sys/mman.h>
#include "us264.h"

#define TIME_STEPS_NUM 2
#define MAX_COLLISION_NUM 5
#define DRONE_NUM 5

void handle_collition_sigurs1() {
    write(STDOUT_FILENO, "\nCollition Detected\n", 20);
}

void setup_sigusr1() {

	struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));

    act.sa_handler = handle_collition_sigurs1;
    act.sa_flags = SA_RESTART;
    sigfillset(&act.sa_mask); // block all other signals

    sigaction(SIGUSR1, &act, NULL);
}

int main() {

	// Allocate the 3D matrix for drone positions dynamically
	Position*** positions_matrix = allocate_position_matrix(DRONE_NUM, TIME_STEPS_NUM);

	initialize_drone_positions(positions_matrix, TIME_STEPS_NUM, DRONE_NUM);

	if (positions_matrix == NULL) {
        return EXIT_FAILURE;
    }

	int fd[DRONE_NUM][2]; //Pipes
	int p[DRONE_NUM];     //PIDs

	// create a pipe for each child
	printf("Setup of pipes in progress...\n\n");
	for (int i = 0; i < DRONE_NUM; i++) {
		if (pipe(fd[i]) == -1) {
			perror("Pipe Failed");
			exit(1);
		}
	}

	int collition_num = 0;

	for (int i = 0; i < DRONE_NUM; i++) {

		p[i] = fork();

		if (p[i] == -1) {
			perror("Fork Failed");
			exit(1);
		}

		if (p[i] == 0) {
			// child/drone process
			close(fd[i][0]); // close read

			setup_sigusr1();

            for (int j = 1; j < TIME_STEPS_NUM; j++) {
            	run_drone_script(fd[i][1], j, positions_matrix, i);
            }

            close(fd[i][1]);  // close write
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

    for (int i = 0; i < DRONE_NUM; i++) {
        close(fd[i][1]);
    }

    //us264
    run_simulation(p, fd, positions_matrix, DRONE_NUM, TIME_STEPS_NUM);

	//close read
    for (int i = 0; i < DRONE_NUM; i++) {
        close(fd[i][0]);
    }

	printf("Simulation finished with success...\n");

	return 0;
}
