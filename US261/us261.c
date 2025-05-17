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

//sig_atomic_t for signals not to interrupt the normal order of program
volatile sig_atomic_t ready_to_move;

void handle_sigcont() {
    ready_to_move = 1;
}

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

void setup_sicont() {

    struct sigaction act2;

    memset(&act2, 0, sizeof(struct sigaction));

    act2.sa_handler = handle_sigcont;
    act2.sa_flags = SA_RESTART;
    sigfillset(&act2.sa_mask);

    sigaction(SIGCONT, &act2, NULL);
}

int main() {

    ready_to_move = 1;

    shared_data_type *shared_data = allocate_shared_memory("/shm_child_collitions");

    for (int i = 0; i < DRONE_NUM; i++) {
        shared_data->child_collitions[i] = 0;
    }

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
			setup_sicont();

            for (int j = 1; j < TIME_STEPS_NUM; j++) {

                while (!ready_to_move) {
                    pause();
                }
                ready_to_move = 0; // reset

            	run_drone_script(fd[i][1], j, positions_matrix, i, TIME_STEPS_NUM);
            }

            close(fd[i][1]);  // close write
            exit(0);
		}
	}

	//=================================================================================================================
	
	//Parent Process



    for (int i = 0; i < DRONE_NUM; i++) {
        close(fd[i][1]);
    }

    //us264
    run_simulation(p, fd, positions_matrix, DRONE_NUM, TIME_STEPS_NUM, *shared_data, MAX_COLLISION_NUM);

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

    deallocate_shared_memory("/shm_child_collitions", shared_data);
    free_position_matrix(positions_matrix, DRONE_NUM, TIME_STEPS_NUM);

	printf("Simulation finished with success...\n");

	return 0;
}
