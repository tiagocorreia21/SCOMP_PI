#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

void verify_collitions(Position *position_matrix, int num_drones, int time_steps, shared_data_type *shared_data, int max_collition_num) {

    for (int t = 0; t < time_steps; t++) {

        for (int i = 0; i < num_drones; i++) {

            for (int j = 0; j < num_drones; j++) {

                if (position_matrix[t][i]->x == position_matrix[t][j]->x &&
                    position_matrix[t][i]->x == position_matrix[t][j]->x &&
                    position_matrix[t][i]->x == position_matrix[t][j]->x) {

                    shared_data->colltion_num++;

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

                    kill(position_matrix[t][i]->pid, SIGUSR1);
                    kill(position_matrix[t][j]->pid, SIGUSR1);
                }
            }
        }
    }
}
