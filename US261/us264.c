#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "functions.h"
#include "us265.h"
#include "us263.h"

void run_simulation(int pids[], int fd[][2], Position ***positions_ptr, int drone_count, int time_steps) {
    //cast do ponteiro para matriz positions[drone_count][time_steps]
    Position ***drone_positions_matrix = positions_ptr;

    for (int t = 0; t < time_steps; t++) {

        printf("\n===== TIME STEP %d =====\n", t);

        //US264 – desbloquear os drones com SIGCONT
        for (int i = 0; i < drone_count; i++) {
            printf("Parent: Sending SIGCONT to drone %d (PID %d)\n", i, pids[i]);
            kill(pids[i], SIGCONT);
        }

        // US262 – receber e armazenar posições
        for (int i = 0; i < drone_count; i++) {

            Position pos;
            
            // Capture the drone movement
            capture_drone_movement(fd[i][0], &pos);

            // verify_collisions(Position ***position_matrix, Position generated_position, int time_step, int drone_num, int *collision_num, int max_collision_num)
            //int c = verify_collition(positions_ptr, pos, t, drone_count, );
            
            // If not the first time step, validate the movement
            if (t > 0) {

                Position current_pos = get_position_3d(drone_positions_matrix, i, t-1, drone_count, time_steps);

                if (!process_movement(&current_pos, &pos)) {

                    printf("Warning: Invalid movement detected for drone %d\n", i);
                    // Use the last valid position
                    pos = current_pos;
                }

            }
            
            // Store the position
            store_position(drone_positions_matrix, i, t, pos);
            //printf("Drone %d @ (%d, %d, %d)\n", i, pos.x, pos.y, pos.z);
        }

        // Print current positions for all drones
        print_positions(drone_positions_matrix, drone_count, t);
    }

    //Chamar US265
	report_generator(positions_ptr, drone_count, time_steps);
}
