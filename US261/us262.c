#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "structs.h"
#include "functions.h"

/**
 * Captures drone movement from the pipe
 * @param fd File descriptor for the pipe
 * @param pos Pointer to store the captured position
 * @return 1 if successful, 0 if failed
 */
void capture_drone_movement(int fd, Position *pos) {
    int n = read(fd, pos, sizeof(Position));
    if (n <= 0) {
        perror("Error reading drone position");
        exit(EXIT_FAILURE);
    }
}

/**
 * Processes and validates a drone movement
 * @param current_pos Current position of the drone
 * @param new_pos New position to validate
 * @return 1 if movement is valid, 0 if invalid
 */
int process_movement(Position *current_pos, Position *new_pos) {
    // Validate that the movement is within reasonable bounds
    // For example, check if the movement is not too large
    int max_movement = 10; // Maximum allowed movement in any direction
    
    int dx = abs(new_pos->x - current_pos->x);
    int dy = abs(new_pos->y - current_pos->y);
    int dz = abs(new_pos->z - current_pos->z);
    
    if (dx > max_movement || dy > max_movement || dz > max_movement) {
        printf("Warning: Drone movement too large (%d, %d, %d)\n", dx, dy, dz);
        return 0;
    }
    
    return 1;
}

/**
 * Stores a drone's position in the 3D position matrix
 * @param positions Pointer to the 3D position matrix
 * @param drone_id The ID of the drone
 * @param time_step The current time step
 * @param pos The position to store
 */
void store_position(Position ***positions_ptr, int drone_id, int time_step, Position pos) {

    // Ensure the matrix and the target location are allocated
    if (positions_ptr == NULL || positions_ptr[time_step] == NULL || positions_ptr[time_step][drone_id] == NULL) {
         fprintf(stderr, "Error: Matrix or specific position not allocated.\n");
        return;
    }

    // Store the position by dereferencing the pointer at the correct location
    *positions_ptr[time_step][drone_id] = pos;

    positions_ptr[time_step][drone_id]->time_step = time_step;
}

/**
 * Prints the current state of all drone positions
 * @param positions Pointer to the 3D position matrix
 * @param drone_count Number of drones
 * @param time_step Current time step
 */
void print_positions(Position *positions_ptr, int drone_count, int time_step) {
    // Cast the pointer to a 3D matrix
    Position (*positions)[time_step] = (Position (*)[time_step])positions_ptr;
    
    printf("\nCurrent positions at time step %d:\n", time_step);
    for (int i = 0; i < drone_count; i++) {
        Position pos = positions[i][time_step];
        printf("Drone %d: (%d, %d, %d)\n", i, pos.x, pos.y, pos.z);
    }
} 