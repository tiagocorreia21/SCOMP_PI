#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "structs.h"
#include "functions.h"

#define DRONE_NUM 5


extern sem_t* step_semaphores[DRONE_NUM];
extern sem_t* main_semaphore;

void simulation(Position *matrix, int time_steps, int num_drone, int max_colis_num) {
	
	
    for (int t = 0; t < time_steps; t++) {
        printf("\n=== SIMULATION TIME STEP %d ===\n", t);

        // Liberta todos os drones para executarem o timestep atual
        for (int d = 0; d < num_drone; d++) {
			
            sem_post(step_semaphores[d]);  // Sinaliza para o drone avançar
        }

        // Espera que todos os drones terminem este timestep
        for (int d = 0; d < num_drone; d++) {
			
            sem_wait(main_semaphore); 	// Drone sinaliza ao terminar
        }

        // Print das posições após todos terem terminado
        print_positions(matrix, num_drone, t);

        //Deteção de colisões, se necessário aqui.
    }
	
}
