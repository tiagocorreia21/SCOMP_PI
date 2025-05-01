#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "functions.h"

#define POSSIBLE_NODES 10
#define TIME_STEPS_NUM 200
#define MAX_COLLISION_NUM 5
#define DRONE_NUM 20

//typedef struct{
//	int positionDrone;
//	int timeStamp;
//}Position;

int main() {
	
	//Array bidimensional para posições dos drones
	Position positions[TIME_STEPS_NUM][DRONE_NUM];

	int fd[DRONE_NUM][2];
	int p[DRONE_NUM];

	//Criação de um pipe para cada filho
	for(int i = 0; i < DRONE_NUM; i++){
		if (pipe(fd[i]) == -1) {
			perror("Pipe Failed");
			exit(1);
		}
	}

	//Cria os filhos(drones)
	for (int i = 0; i < DRONE_NUM; i++) {

		p[i] = fork();

		if (p[i] == -1) {
			perror("Fork Failed");
			exit(1);
		}

		if (p[i] == 0) {
			
			close(fd[i][0]);
			
			//Tentativa para simular escrita do drone
            Position pos;
            
            for (int t = 0; t < TIME_STEPS_NUM; t++) {
                pos.positionDrone = i; 
                pos.timeStamp = t;
                write(fd[i][1], &pos, sizeof(Position));
                usleep(50000); //Para simular tempo (50ms)
            }
            
            close(fd[i][1]);
            exit(0);
		}
	}
	
	//Processo pai
	
	//Fechar lados de escrita
    for (int i = 0; i < DRONE_NUM; i++) {
        close(fd[i][1]);
    }

	for (int i = 0; i < TIME_STEPS_NUM; i++){
		for(int j = 0; j < DRONE_NUM; j++){
			
			Position pos;
			//Processo pai
			if(p[j] > 0) {
				read(fd[j][0], &pos, sizeof(Position));
				positions[i][j] = pos;
			}	
			
		}
	}
	//Fechar os lados da leitura
	for (int j = 0; j < DRONE_NUM; j++) {
		close(fd[j][0]);
	}
	
	return 0;
}
