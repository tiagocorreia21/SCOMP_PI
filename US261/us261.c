#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "functions.h"

#define POSSIBLE_NODES 10
#define TIME_STEPS_NUM 200
#define MAX_COLLISION_NUM 5
#define DRONE_NUM 20

int main() {
	
	//Array bidimensional para posições dos drones
	Position positions[TIME_STEPS_NUM][DRONE_NUM];

	int fd[DRONE_NUM][2];
	int p[DRONE_NUM];

	// create a pipe for each child
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

            Position pos;

            // simulate writing by child/drone
            for (int t = 0; t < TIME_STEPS_NUM; t++) {

                pos.time_stamp = t;

                int n = write(fd[i][1], &pos, sizeof(Position));

                if (n == -1) {
                	perror("Write Failed");
                	exit(1);
                }
                usleep(50000); // simulate time (50ms)
            }
            
            close(fd[i][1]);  // close write
            exit(0);

		} else {
			// store the child pids
			p[i] = p;
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

		//TODO : add a logic for termination codes so child processes
		if (WIFEXITED(status)) {

		}
	}
	
	return 0;
}
