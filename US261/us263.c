#include <signal.h>
#include <string.h>
#include <unistd.h>

void verify_collitions(Position *position_matrix, int num_drones, int time_steps, shared_data_type *shared_data) {

    for (int t = 0; i < time_steps; i++) {

        for (int i = 0; i < num_drones; i++) {

            for (int j = 0; j < num_drones; j++) {

                if (position_matrix[t][i]->x == position_matrix[t][j]->x &&
                    position_matrix[t][i]->x == position_matrix[t][j]->x &&
                    position_matrix[t][i]->x == position_matrix[t][j]->x) {

                    shared_data->colltion_num++;

                    kill(position_matrix[t][i]->pid, SIGUSR1);
                    kill(position_matrix[t][j]->pid, SIGUSR1);
                }
            }
        }
    }
}
