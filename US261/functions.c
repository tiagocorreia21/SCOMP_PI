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

void initialize_drone_positions(Position *positions, int time_step_num, int drone_num) {

	for (int i = 0; i < time_step_num; i++) {

		for (int j = 0; j < drone_num; j++) {

			positions[i * drone_num + j].x = -1; // -1 indicates undefined position
            positions[i * drone_num + j].y = -1;
            positions[i * drone_num + j].z = -1;
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
