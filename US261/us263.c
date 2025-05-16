#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "structs.h"
#include <stdlib.h>
#include "functions.h"

/**
 * Verify Collisions
 * @param Position Matrix
 * @param Generated Position
 * @param Time Step
 * @param Number of Drones
 * @param Number of collisions of the current drone
 * @param Max collisions allowed for each drone
 * @return 1 if collision, 0 if no collision
 */
int verify_collisions(Position ***position_matrix, Position generated_position, int time_step, int drone_num, int *collision_num, int max_collision_num) {

	for (int i = 0; i < drone_num; i++) {

		Position pos = get_position_3d(position_matrix, i, time_step, drone_num, time_step);

		if (pos.x == generated_position.x || pos.y == generated_position.y || pos.z == generated_position.z) {

			if (*collision_num >= max_collision_num) {
				printf("Drone Process PID %d terminating. Collision Threshold exceeded\n", getpid());
				raise(SIGINT);
			}

			//(*collition_num)++;

            FILE *file = fopen("collitions_logs.txt", "w");

            if (file == NULL) {
            	perror("fopen failed");
                exit(1);
            }

            fprintf(file, "Collision detected between generated position and drone %d at time step %d\n", i, time_step);

            fclose(file);

            return 1;
        }
    }
    return 0;
}
