#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "structs.h"
#include <stdlib.h>
#include "functions.h"

/**
 * Verify Collitions
 * @param Position Matrix
 * @param Generated Position
 * @param Time Step
 * @param Number of Drones
 * @param Number of collition of the current drone
 * @param Max collitions allowed for each drone
 * @return 1 if collition, 0 if no collition
 */
int verify_collitions(Position ***position_matrix, Position generated_position, int time_step, int drone_num, int *collition_num, int max_collition_num) {

	for (int i = 0; i < drone_num; i++) {

		Position pos = get_position_3d(position_matrix, i, time_step, drone_num, time_step);

		if (pos.x == generated_position.x || pos.y == generated_position.y || pos.z == generated_position.z) {

			if (*collition_num >= max_collition_num) {
				printf("Drone Process PID %d terminating. Collition Threshold exceeded\n", getpid());
				raise(SIGINT);
			}

			(*collition_num)++;

            FILE *file = fopen("collitions_logs.txt", "w");

            if (file == NULL) {
            	perror("fopen failed");
                exit(1);
            }

            fprintf(file, "Collition detected between generated position and drone %d at time step %d\n", i, time_step);

            fclose(file);

            return 1;
        }
    }
    return 0;
}
