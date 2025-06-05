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
#define INVALID_POSITION -999

void run_drone_script(int time_step, Position *matrix, int drone_id) {

    printf("Drone Script\n");
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
        Position pos = get_position(matrix, i, time_step, drone_num);
        printf("Drone %d (%d, %d, %d)\n", i, pos.x, pos.y, pos.z);
    }
}

