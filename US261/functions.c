#include <stdlib.h>
#include "structs.h"
#include <unistd.h>
#include <stdio.h>

#define SPACE_X 1000
#define SPACE_Y 1000
#define SPACE_Z 1000

void run_drone_script(int write_fd, int time_step_num) {
    // drone script - simulate drone movements

    // TODO - implement the actual movements of the drones

    // Example script with random movements
    for (int i = 0; i < time_step_num; i++) {

    	Position position;

    	position.x = rand() % SPACE_X;
    	position.y = rand() % SPACE_Y;
    	position.z = rand() % SPACE_Z;
    	position.time_step = i;

    	int n = write(write_fd, &position, sizeof(position));

    	if (n == -1) {
    		perror("write");
    		exit(EXIT_FAILURE);
    	}
    }
}

void initialize_drone_positions(Position *positions, int time_step_num, int drone_num) {

	for (int i = 0; i < time_step_num; i++) {

		for (int j = 0; j < drone_num; j++) {

			positions[i * drone_num + j].x = -1; // -1 indica posição indefinida
            positions[i * drone_num + j].y = -1;
            positions[i * drone_num + j].z = -1;
		}
	}
}
