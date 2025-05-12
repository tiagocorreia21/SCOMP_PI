#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "structs.h"
#include "functions.h"

#define POSSIBLE_NODES 10
#define TIME_STEPS_NUM 5
#define MAX_COLLISION_NUM 5
#define DRONE_NUM 10

int main() {

    shared_data_type *shared_data = allocate_shared_memory("/shm_collitions");

	// Allocate the 3D matrix for drone positions dynamically
	Position*** positions_matrix = allocate_position_matrix(DRONE_NUM, TIME_STEPS_NUM);

	printf("Initializing drone positions\n\n");
	initialize_drone_positions(**positions_matrix, TIME_STEPS_NUM, DRONE_NUM);

	if (positions_matrix == NULL) {
        // Error message already printed in allocate_position_matrix
        // Need to clean up shared memory here if it was allocated successfully
        // free_shared_memory(shared_data, "/shm_collitions");
        return EXIT_FAILURE;
    }

	int fd[DRONE_NUM][2]; //Pipes
	int p[DRONE_NUM]; //PIDs

	// create a pipe for each child
	printf("Setup of pipes in progress...\n\n");
	for (int i = 0; i < DRONE_NUM; i++) {
		if (pipe(fd[i]) == -1) {
			perror("Pipe Failed");
			exit(1);
		}
	}

	for (int i = 0; i < DRONE_NUM; i++) {

		p[i] = fork();

		if (p[i] == -1) {
			perror("Fork Failed");
			exit(1);
		}

		if (p[i] == 0) {
			// child/drone process
			close(fd[i][0]); // close read

            // simulate writing by child/drone
            run_drone_script(fd[i][1], TIME_STEPS_NUM);

            close(fd[i][1]);  // close write
            exit(0);
		}
	}

	//=================================================================================================================
	
	//Parent Process

	// Fechar escrita no pai (só vai ler)
    for (int i = 0; i < DRONE_NUM; i++) {
        close(fd[i][1]);
    }

    //us264
    run_simulation(p, fd, positions_matrix, DRONE_NUM, TIME_STEPS_NUM, shared_data);

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
	
	//close read
    for (int i = 0; i < DRONE_NUM; i++) {
        close(fd[i][0]);
    }

	printf("Simulation finished with success...\n");
	return 0;
}
