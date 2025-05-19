#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "functions.h"
#include "us265.h"
#include "us263.h"

void run_simulation(int pids[], int fd[][2], Position ***positions_ptr, int drone_count, int time_steps, shared_data_type shared_data, int max_collition_num) {

    Position ***drone_positions_matrix = positions_ptr;

    for (int t = 0; t < time_steps; t++) {

        //US264
        for (int i = 0; i < drone_count; i++) {
            //printf("Parent: Sending SIGCONT to drone %d (PID %d)\n", i, pids[i]);
            kill(pids[i], SIGCONT);
        }

        // US262
        for (int i = 0; i < drone_count; i++) {

            Position pos;

            capture_drone_movement(fd[i][0], &pos);
            //printf("Parent: Received position from drone %d: (%d, %d, %d)\n", i, pos.x, pos.y, pos.z);

            int drone_id = pos.drone_id;

			
            int collition_result = verify_collisions(positions_ptr, pos, t, drone_count, time_steps);

            if (collition_result == 1) {
				
                shared_data.child_collitions[drone_id]++;

                kill(pos.pid, SIGUSR1);

                if (shared_data.child_collitions[drone_id] >= max_collition_num) {
                    kill(pos.pid, SIGINT);
                }
            }

            // If not the first time step, validate the movement
            /*if (t > 0) {

                Position current_pos = get_position_3d(drone_positions_matrix, i, t-1, drone_count, time_steps);

                if (!process_movement(&current_pos, &pos)) {

                    printf("Warning: Invalid movement detected for drone %d\n", i);
                    // Use the last valid position
                    pos = current_pos;
                }
            }*/

            store_position(drone_positions_matrix, i, t, pos);
            
        }

        // Print current positions for all drones
        print_positions(drone_positions_matrix, drone_count, t);
    }

    //Chamar US265
	report_generator(positions_ptr, drone_count, time_steps);
}
