#include <stdlib.h>
#include "structs.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>

#define SPACE_X 1000
#define SPACE_Y 1000
#define SPACE_Z 1000

//sig_atomic_t para sinais nao interromperem fluxo normal do programa
volatile sig_atomic_t ready_to_move = 0;

void handler(int sig) {
    ready_to_move = 1;
}

void run_drone_script(int write_fd, int time_step_num) {

    signal(SIGCONT, handler);

    // Example script with random movements
    for (int i = 0; i < time_step_num; i++) {

        // Espera por sinal do processo pai
        while (!ready_to_move) {
            pause();
        }
        ready_to_move = 0; // reset


    	Position position;

    	position.x = rand() % SPACE_X;
    	position.y = rand() % SPACE_Y;
    	position.z = rand() % SPACE_Z;
    	position.time_step = i;

    	int n = write(write_fd, &position, sizeof(position));

    	if (n == -1) {
    		perror("write");
    		exit(EXIT_FAILURE);
    	}
    }
}

shared_data_type *allocate_shared_memory(char *shm_name) {

    int data_size = sizeof(shared_data_type);

    shared_data_type *shared_data;

    int fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        perror("shm_open failed");
        exit(1);
    }

    int ftuncate = ftruncate(fd, data_size);

    if (ftuncate == -1) {
        perror("ftruncate failed");
        exit(1);
    }

    shared_data = (shared_data_type *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    return shared_data;
}

void deallocate_shared_memory(char *shm_name, shared_data_type *shared_data) {
	int data_size = sizeof(shared_data_type);
	munmap(shared_data, data_size);
	//close(fd);
	shm_unlink(shm_name);
}

// Função para alocar a matriz 3D de posições
Position*** allocate_position_matrix(int num_drones, int time_steps) {
    Position*** matrix = (Position***)malloc(time_steps * sizeof(Position**));
    if (matrix == NULL) {
        perror("Failed to allocate memory for time steps");
        return NULL;
    }

    for (int t = 0; t < time_steps; t++) {
        matrix[t] = (Position**)malloc(num_drones * sizeof(Position*));
        if (matrix[t] == NULL) {

            perror("Failed to allocate memory for drones at a time step");

            // Limpar a memória já alocada
            for (int i = 0; i < t; i++) {
                free(matrix[i]);
            }
            free(matrix);
            return NULL;
        }
        for (int d = 0; d < num_drones; d++) {
            matrix[t][d] = (Position*)malloc(sizeof(Position));
            if (matrix[t][d] == NULL) {

                perror("Failed to allocate memory for a position");

                // Limpar a memória já alocada
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

            // Inicializar a posição
            matrix[t][d]->x = 0;
            matrix[t][d]->y = 0;
            matrix[t][d]->z = 0;
            matrix[t][d]->time_step = t;
        }
    }
    return matrix;
}

// Função para liberar a matriz 3D de posições
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

Position get_position_3d(Position*** matrix, int drone_id, int time_step, int num_drones, int time_steps_num) {

    // Basic boundary checks
    if (drone_id < 0 || drone_id >= num_drones || time_step < 0 || time_step >= time_steps_num) {

         fprintf(stderr, "Error: Attempted to get position out of matrix bounds (drone %d, time %d).\n", drone_id, time_step);

         Position invalid_position;
         invalid_position.x = -1;
         invalid_position.y = -1;
         invalid_position.z = -1;

         return invalid_position;
    }

    // Ensure the memory is allocated at this location
    if (matrix == NULL || matrix[time_step] == NULL || matrix[time_step][drone_id] == NULL) {

         fprintf(stderr, "Error: Matrix or specific position not allocated at (drone %d, time %d).\n", drone_id, time_step);

         // Return a default or invalid position
         return (Position){-1, -1, -1}; // Example with -1 for invalid coordinates
    }

    // Return the Position struct by dereferencing the pointer at the correct location
    return *matrix[time_step][drone_id];
}

Position get_position(Position *positions_ptr, int drone_id, int time_step) {
    // Cast the pointer to a 3D matrix
    Position (*positions)[time_step] = (Position (*)[time_step])positions_ptr;
    
    return positions[drone_id][time_step];
}

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

void initialize_drone_positions(Position ***positions_matrix, int time_step_num, int drone_num) {
    // Initialize positions for each drone at each time step
    for (int t = 0; t < time_step_num; t++) {
        for (int d = 0; d < drone_num; d++) {
            // Initialize with random positions between 0 and 100
            positions_matrix[t][d]->x = rand() % 100;
            positions_matrix[t][d]->y = rand() % 100;
            positions_matrix[t][d]->z = rand() % 100;
            positions_matrix[t][d]->time_step = t;
        }
    }
} 