#include "../US261/structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <string.h>


/**
 * Simulates and tracks drone movements over time
 * @param num_drones Number of drones to simulate
 * @param time_steps Number of time steps to track
 * @param history_size Number of past positions to keep for collision detection
 * @return 0 on success, -1 on failure
 */
int movement_drones(int num_drones, int time_steps, int history_size) {
    // Communication structures
    int pipe_fds[num_drones][2];        // Pipe file descriptors for each drone [drone_idx][read/write]
    pid_t drone_pids[num_drones];       // Process IDs for each drone
    int drone_read_fds[num_drones];     // Read ends of pipes for select()
    
    // Position tracking
    TimeStep time_steps_array[time_steps];  // Array to store positions for all drones at each time step
    int current_time_step = 0;              // Current simulation time step
    
    // Initialize time steps array with default values
    for (int t = 0; t < time_steps; t++) {
        for (int d = 0; d < num_drones; d++) {
            time_steps_array[t].drone_positions[d].x = -1;  // -1 indicates undefined position
            time_steps_array[t].drone_positions[d].y = -1;
            time_steps_array[t].drone_positions[d].z = -1;
            time_steps_array[t].drone_positions[d].time_step = t;
        }
    }

    // Create pipes for inter-process communication
    for (int i = 0; i < num_drones; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("pipe");
            return -1;
        }
    }

    // Create drone processes
    for (int i = 0; i < num_drones; i++) {
        drone_pids[i] = fork();

        if (drone_pids[i] == -1) {
            perror("fork");
            return -1;
        }

        if (drone_pids[i] == 0) { // Child process (Drone)
            // Close unused pipe ends
            close(pipe_fds[i][0]);
            
            // Close other drones' pipes to prevent resource leaks
            for (int j = 0; j < num_drones; j++) {
                if (i != j) {
                    close(pipe_fds[j][0]);
                    close(pipe_fds[j][1]);
                }
            }

            // Simulate drone movement and send position updates
            for (int t = 0; t < time_steps; t++) {
                Position pos;
                // Generate random position in 1000x1000x1000 space
                pos.x = rand() % 1000;
                pos.y = rand() % 1000;
                pos.z = rand() % 1000;
                pos.time_step = t;

                // Send position update to main process
                if (write(pipe_fds[i][1], &pos, sizeof(Position)) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
                
                usleep(100000);  // Simulate time between movements (100ms)
            }

            close(pipe_fds[i][1]);
            exit(EXIT_SUCCESS);
        } else { // Parent process
            close(pipe_fds[i][1]);  // Close write end
            drone_read_fds[i] = pipe_fds[i][0];  // Store read end for select()
        }
    }

    // Main process: Read and process drone positions
    fd_set read_fds;
    int max_fd = 0;
    
    // Find the maximum file descriptor for select()
    for (int i = 0; i < num_drones; i++) {
        if (drone_read_fds[i] > max_fd) {
            max_fd = drone_read_fds[i];
        }
    }

    // Main simulation loop
    while (current_time_step < time_steps) {
        // Initialize file descriptor set for select()
        FD_ZERO(&read_fds);
        for (int i = 0; i < num_drones; i++) {
            FD_SET(drone_read_fds[i], &read_fds);
        }

        // Wait for data from any drone
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            return -1;
        }

        // Process data from ready drones
        for (int i = 0; i < num_drones; i++) {
            if (FD_ISSET(drone_read_fds[i], &read_fds)) {
                Position pos;
                if (read(drone_read_fds[i], &pos, sizeof(Position)) == sizeof(Position)) {
                    // Store the position in the time-indexed array
                    time_steps_array[pos.time_step].drone_positions[i] = pos;
                    
                    // Print position update
                    printf("Time %d: Drone %d at (%d, %d, %d)\n", 
                           pos.time_step, i, pos.x, pos.y, pos.z);
                    
                    // Check for potential collisions with past positions
                    if (pos.time_step >= history_size) {
                        for (int past = pos.time_step - history_size; past < pos.time_step; past++) {
                            for (int other = 0; other < num_drones; other++) {
                                if (other != i) {
                                    Position past_pos = time_steps_array[past].drone_positions[other];
                                    // Check if positions match (collision)
                                    if (past_pos.x == pos.x && past_pos.y == pos.y && past_pos.z == pos.z) {
                                        printf("WARNING: Potential collision detected between Drone %d and Drone %d at time %d\n",
                                               i, other, pos.time_step);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        current_time_step++;
    }

    // Clean up: wait for all drone processes and close pipes
    for (int i = 0; i < num_drones; i++) {
        waitpid(drone_pids[i], NULL, 0);
        close(drone_read_fds[i]);
    }

    return 0;
}