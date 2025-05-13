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

void handle_collition_sigurs1(int signo) {
    write(STDOUT_FILENO, "\nCollition Detected\n", 20);
    printf("(%d)", signo);
}

void handle_sigint(int signo) {

	write(STDOUT_FILENO, "\nSimulation terminated: collition threshold exceeded\n", 53);
	printf("(%d)", signo);

	//deallocate_shared_memory("shm_collitions", shared_data);

	exit(0);
}

int main() {
	shm_unlink("/shm_collitions");
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));

	act.sa_handler = handle_sigint;
	act.sa_flags = SA_RESTART;
	sigfillset(&act.sa_mask);

	sigaction(SIGINT, &act, NULL);
	
    shared_data_type *shared_data = allocate_shared_memory("/shm_collitions");

	// Allocate the 3D matrix for drone positions dynamically
	Position*** positions_matrix = allocate_position_matrix(DRONE_NUM, TIME_STEPS_NUM);


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

            struct sigaction act;

            memset(&act, 0, sizeof(struct sigaction));

            act.sa_handler = handle_collition_sigurs1;

            // restard interrupted system calls
            act.sa_flags = SA_RESTART;

            // block all other signals
            sigfillset(&act.sa_mask);

            sigaction(SIGUSR1, &act, NULL);

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
    run_simulation(p, fd, positions_matrix, DRONE_NUM, TIME_STEPS_NUM, shared_data, MAX_COLLISION_NUM);

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
	shm_unlink("/shm_collitions");
	return 0;
}
