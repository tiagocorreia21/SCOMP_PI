#include <stdlib.h>
#include "structs.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define SPACE_X 50
#define SPACE_Y 50
#define SPACE_Z 50
#define INVALID_POSITION -999

Position generate_position(Position *matrix, int time_step, int drone_id, int drone_num) {
	
	Position last_position;
	srand(time(NULL) + drone_id + time_step * 100);

   
    if (time_step == 0) {
		int dx = rand() % SPACE_X;
        int dy = rand() % SPACE_Y;
        int dz = rand() % SPACE_Z;

        matrix[POS_IDX(0, drone_id, drone_num)].x = dx;
        matrix[POS_IDX(0, drone_id, drone_num)].y = dy;
        matrix[POS_IDX(0, drone_id, drone_num)].z = dz;

        last_position = matrix[POS_IDX(0, drone_id, drone_num)];
            
    } else {
        last_position = matrix[POS_IDX(time_step - 1, drone_id, drone_num)];
    }

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

void run_drone_script(int time_step, Position *matrix, int drone_id, int drone_num) {

	Position new_position = generate_position(matrix, time_step, drone_id, drone_num);

	printf("Drone PID: (%d) | New Drone Position: (%d, %d, %d)\n", getpid(),new_position.x, new_position.y, new_position.z);

	matrix[POS_IDX(time_step, drone_id, drone_num)] = new_position;
}

Position* allocate_shared_position_matrix(int num_drones, int time_steps, char shm_name[]) {

    int data_size = num_drones * time_steps * sizeof(Position);

    Position* matrix;

    int fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        perror("shm_open failed");
        exit(4);
    }

    int ftuncate = ftruncate(fd, data_size);

    if (ftuncate == -1) {
        perror("ftruncate failed");
        exit(5);
    }

    matrix = (Position *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (matrix == MAP_FAILED) {
        perror("mmap failed");
        exit(6);
    }
    return matrix;
}

void free_position_matrix(Position *matrix, char shm_name[], int drone_num, int time_step_num) {

    int data_size = drone_num * time_step_num * sizeof(Position);

    if (munmap(matrix, data_size) == -1) {
        perror("munmap failed");
        exit(7);
    }

    if (shm_unlink(shm_name) == -1) {
        perror("shm_unlink failed");
        exit(8);
    }
}

void store_position(Position *matrix ,int drone_id, int time_step, int drone_num, Position new_pos) {

    int idx = POS_IDX(time_step, drone_id, drone_num);

    matrix[idx] = new_pos;
}

Position get_position(Position* matrix, int drone_id, int time_step, int drone_num) {

    int idx = POS_IDX(time_step, drone_id, drone_num);

    return matrix[idx];
}

void print_positions(Position *matrix, int drone_num, int time_step) {

    printf("\n===== TIME STEP %d =====\n", time_step);

    for (int i = 0; i < drone_num; i++) {
        Position pos = matrix[POS_IDX(time_step, i, drone_num)];
        printf("Drone %d (%d, %d, %d)\n", i, pos.x, pos.y, pos.z);
    }
}

