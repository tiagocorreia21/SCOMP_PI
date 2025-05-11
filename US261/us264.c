#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "functions.h"


void run_simulation(int pids[], int fd[][2], Position *positions_ptr, int drone_count, int time_steps, shared_data_type *shared_data) {
    //cast do ponteiro para matriz positions[drone_count][time_steps]
    Position (*positions)[time_steps] = (Position (*)[time_steps])positions_ptr;

    for (int t = 0; t < time_steps; t++) {
        printf("\n===== TIME STEP %d =====\n", t);

        //US264 – desbloquear os drones com SIGCONT
        for (int i = 0; i < drone_count; i++) {
            kill(pids[i], SIGCONT);
        }

        // US262 – receber e armazenar posições
        for (int i = 0; i < drone_count; i++) {
            Position pos;
            
            // Capture the drone movement
            capture_drone_movement(fd[i][0], &pos);
            
            // If not the first time step, validate the movement
            if (t > 0) {
                Position current_pos = get_position(positions_ptr, i, t-1);
                if (!process_movement(&current_pos, &pos)) {
                    printf("Warning: Invalid movement detected for drone %d\n", i);
                    // Use the last valid position
                    pos = current_pos;
                }
            }
            
            // Store the position
            store_position(positions_ptr, i, t, pos);
            printf("Drone %d @ (%d, %d, %d)\n", i, pos.x, pos.y, pos.z);
        }

        // Print current positions for all drones
        print_positions(positions_ptr, drone_count, t);

        // US263 – detetar colisões
        //detect_collisions(positions, t, drone_count, pids, shared_data);
    }

    //Chamar US265
}
