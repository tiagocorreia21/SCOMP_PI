#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "structs.h"
#include "functions.h"

#define POSSIBLE_NODES 10
#define TIME_STEPS_NUM 200
#define MAX_COLLISION_NUM 5
#define DRONE_NUM 20

int main() {

	//Array to store drone positions
	Position positions[TIME_STEPS_NUM][DRONE_NUM];

	printf("Initializing drone positions\n");
	initialize_drone_positions(positions, TIME_STEPS_NUM, DRONE_NUM);

	int fd[DRONE_NUM][2];
	int p[DRONE_NUM];

	// create a pipe for each child
	printf("Setup of pipes in progress...\n");
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
            run_drone_script(getpid(), fd[i][1], TIME_STEPS_NUM);

            close(fd[i][1]);  // close write
            exit(0);

		} else {
			// store the child pids
			// p[i] = p;
		}
	}

	//=================================================================================================================
	
	//Parent Process
	
	//close write
    for (int i = 0; i < DRONE_NUM; i++) {
        close(fd[i][1]);
    }

	for (int i = 0; i < TIME_STEPS_NUM; i++) {

		for (int j = 0; j < DRONE_NUM; j++) {
			
			Position pos;

			int n = read(fd[j][0], &pos, sizeof(Position));

			if (n == -1) {
				perror("Read Failed");
				exit(1);
			}

			positions[i][j] = pos;

			printf("Drone %d position: (%d, %d, %d)\n)", i, pos.x, pos.y, pos.z);
		}
	}

	//close read
	for (int j = 0; j < DRONE_NUM; j++) {
		close(fd[j][0]);
	}

	// wait for all the child process to finish
	for (int i = 0; i < DRONE_NUM; i++) {

		int status;

		int finished_pid = waitpid(p[i], &status, 0);

		//TODO : add a logic for termination codes to child processes
		if (WIFEXITED(status)) {

		}
		printf("Drone %d finished\n", i);
	}

	printf("Simulation finished with success...\n");
	return 0;
}
