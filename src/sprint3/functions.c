#include <stdlib.h>
#include "structs.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define SPACE_X 50
#define SPACE_Y 50
#define SPACE_Z 50
#define TIME_STEPS_NUM 6
#define DRONE_NUM 10
#define INVALID_POSITION -999

Position generate_position2(Position ***position_matrix, int drone_id, int time_step) {

    Position last_position;

    // Se for o primeiro timestep, usamos a posição atual (foi inicializada)
    if (time_step == 0) {
        last_position = *position_matrix[0][drone_id];
    } else {
        last_position = *position_matrix[time_step - 1][drone_id];
    }

    srand(time(NULL) + drone_id + time_step * 100);

    int dx = (rand() % 3) - 1; // -1, 0, 1
    int dy = (rand() % 3) - 1;
    int dz = (rand() % 3) - 1;

    Position next_position;

    next_position.x = last_position.x + dx;
    next_position.y = last_position.y + dy;
    next_position.z = last_position.z + dz;


    // Garantir que está dentro dos limites
    if (next_position.x < 0) next_position.x = 0;
    if (next_position.y < 0) next_position.y = 0;
    if (next_position.z < 0) next_position.z = 0;
    if (next_position.x >= SPACE_X) next_position.x = SPACE_X - 1;
    if (next_position.y >= SPACE_Y) next_position.y = SPACE_Y - 1;
    if (next_position.z >= SPACE_Z) next_position.z = SPACE_Z - 1;

    return next_position;
}

void run_drone_script(int time_step, Position ***position_matrix, int drone_id) {

    printf("Drone Script\n");
}

Position*** allocate_position_matrix(int num_drones, int time_steps) {

    Position*** matrix = (Position***) malloc(time_steps * sizeof(Position**));

    if (matrix == NULL) {
        perror("Failed to allocate memory for time steps");
        return NULL;
    }

    srand(time(NULL) + time_steps);

    for (int t = 0; t < time_steps; t++) {

        matrix[t] = (Position **) malloc(num_drones * sizeof(Position*));

        if (matrix[t] == NULL) {

            perror("Failed to allocate memory for drones at a time step");

            for (int i = 0; i < t; i++) {

            	for (int d = 0; d < num_drones; d++) {
            		free(matrix[i][d]);
            	}

                free(matrix[i]);
            }
            free(matrix);

            return NULL;
        }

        for (int d = 0; d < num_drones; d++) {

            matrix[t][d] = (Position*) malloc(sizeof(Position));

            if (matrix[t][d] == NULL) {

                perror("Failed to allocate memory for a position");

                for (int j = 0; j < d; j++) {
                    free(matrix[t][j]);
                }
                free(matrix[t]);

                for (int i = 0; i < t; i++) {

                    for (int k = 0; k < num_drones; k++) {
                        free(matrix[i][k]);
                    }
                    free(matrix[i]);
                }
                free(matrix);

                return NULL;
            }

			if (t == 0) {
                int dx = rand() % SPACE_X;
                int dy = rand() % SPACE_Y;
                int dz = rand() % SPACE_Z;

                matrix[0][d]->x = dx;
                matrix[0][d]->y = dy;
                matrix[0][d]->z = dz;
            } else {
                matrix[t][d]->x = 0;
                matrix[t][d]->y = 0;
                matrix[t][d]->z = 0;
            }
            matrix[t][d]->time_step = t;
            matrix[t][d]->drone_id = d;
        }
    }

    return matrix;
}

void free_position_matrix(Position*** matrix, int num_drones, int time_steps) {

    if (matrix == NULL) return;

    for (int t = 0; t < time_steps; t++) {

        if (matrix[t] == NULL) continue;

        for (int d = 0; d < num_drones; d++) {
            free(matrix[t][d]);
        }
        free(matrix[t]);
    }
    free(matrix);
}

/**
 * Captures drone movement from the pipe
 * @param fd File descriptor for the pipe
 * @param pos Pointer to store the captured position
 * @return 1 if successful, 0 if failed
 */
void capture_drone_movement(int fd, Position *pos) {

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
void print_positions(Position ***positions_ptr, int drone_count, int time_step) {

    printf("\n===== TIME STEP %d =====\n", time_step);

    for (int i = 0; i < drone_count; i++) {
        Position pos = *positions_ptr[time_step][i];
        printf("Drone %d @ (%d, %d, %d)\n", i, pos.x, pos.y, pos.z);
    }
}

