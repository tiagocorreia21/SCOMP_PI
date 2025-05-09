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
            int n = read(fd[i][0], &pos, sizeof(Position));

            if (n <= 0) {
                perror("read (posição)");
                exit(EXIT_FAILURE);
            }
            //US262 - guardar posição recebida na matriz
            //store_position(positions, i, t, pos);
            printf("Drone %d @ (%d, %d, %d)\n", i, pos.x, pos.y, pos.z);
        }

        // US263 – detetar colisões
        //detect_collisions(positions, t, drone_count, pids, shared_data);
    }

    //Chamar US265
}
