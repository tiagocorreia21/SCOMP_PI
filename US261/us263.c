#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "structs.h"
#include <stdlib.h>
#include "functions.h"

void verify_collitions(Position ***position_matrix, int num_drones, int time_steps, shared_data_type *shared_data, int max_collition_num) {

    for (int t = 0; t < time_steps; t++) {

        for (int i = 0; i < num_drones; i++) {

            for (int j = 0; j < num_drones; j++) {

            	Position pos_i = get_position_3d(position_matrix, i, t, num_drones, time_steps);
            	Position pos_j = get_position_3d(position_matrix, j, t, num_drones, time_steps);

				if (pos_i.x == pos_j.x || pos_i.y == pos_j.y || pos_i.z == pos_j.z) {

					shared_data->collition_num++;

		    		if (shared_data->collition_num == max_collition_num) {
                    	raise(SIGINT);
            		}

                    FILE *file = fopen("collitions_logs.txt", "w");

                    if (file == NULL) {
                    	perror("fopen failed");
                    	exit(1);
                    }

                    fprintf(file, "Collition Detected on dornes %d and %d on time step %d\n", i, j, t);

                    fclose(file);

                    //kill(pos_i.pid, SIGUSR1);
                    //kill(pos_j.pid, SIGUSR1);
                }
			}
        }
    }
}
