#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <structs.h>
#include <functions.h>

#define DRONE_NUM 50

int main() {
	
	int fd[2];

	int p[DRONE_NUM];

	if (pipe(fd) == -1) {
		perror("Pipe Failed");
		exit(1);
	}

	// create childs - drones
	for (int i = 0; i < DRONE_NUM; i++) {

		p[i] = fork();

		if (p[i] == -1) {
			perror("Fork Failed");
			exit(1);
		}

		if (p[i] == 0) {

			// child/drone process

			exit(0);
		}
	}
	
	// parent process
	
	
	return 0;
}
