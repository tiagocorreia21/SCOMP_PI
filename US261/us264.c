#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "structs.h"
#include "functions.h"
#include "us265.h"
#include "us263.h"

void run_simulation(int pids[], int fd[][2], Position ***positions_ptr, int drone_count, int time_steps, shared_data_type shared_data, int max_collition_num) {

    Position ***drone_positions_matrix = positions_ptr;
	
	bool drone_active[drone_count];
	for (int i = 0; i < drone_count; i++) drone_active[i] = true;
		
	for (int t = 0; t < time_steps; t++) {
        printf("\n===== TIME STEP %d =====\n", t);

        for (int i = 0; i < drone_count; i++) {
            if (drone_active[i]) kill(pids[i], SIGCONT);
            sleep(1);
        }

        for (int i = 0; i < drone_count; i++) {
            if (!drone_active[i]) continue;

            Position pos;
            int n = read(fd[i][0], &pos, sizeof(Position));

            if (n <= 0) {
                perror("Erro ao ler posicao do drone");
                drone_active[i] = false;
                continue;
            }

            int drone_id = pos.drone_id;

            int collition_result = verify_collisions(positions_ptr, pos, t, drone_count, time_steps);
            if (collition_result == 1) {
                shared_data.child_collitions[drone_id]++;
                kill(pos.pid, SIGUSR1);
                if (shared_data.child_collitions[drone_id] >= max_collition_num) {
                    kill(pos.pid, SIGINT);
                    drone_active[drone_id] = false;
                }
            }

            if (t > 0) {
                Position current_pos = get_position_3d(drone_positions_matrix, i, t-1, drone_count, time_steps);
                if (!process_movement(&current_pos, &pos)) {
                    printf("Warning: Invalid movement detected for drone %d\n", i);
                    pos = current_pos;
                }
            }

            store_position(drone_positions_matrix, i, t, pos);
        }

        print_positions(drone_positions_matrix, drone_count, t);
    }

    report_generator(positions_ptr, drone_count, time_steps);
}
