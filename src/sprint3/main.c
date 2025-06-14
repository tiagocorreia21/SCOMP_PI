#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include "thread_functions.h"
#include "structs.h"
#include "functions.h"
#include "us364.h"

#define TIME_STEPS_NUM 5
#define MAX_COLLISION_NUM 2
#define DRONE_NUM 10
#define THREAD_NUM 2

sem_t* step_semaphores[DRONE_NUM];
sem_t* main_semaphore;

int main() {

	Position* matrix = allocate_shared_position_matrix(DRONE_NUM, TIME_STEPS_NUM, "/matrix_shared_data");

	if (matrix == NULL) {
	    perror("matrix allocation failed");
        exit(10);
    }

	for (int i = 0; i < DRONE_NUM; i++) {
		char sem_name[64];
		snprintf(sem_name, sizeof(sem_name), "/step_semaphore_%d", i);
		sem_unlink(sem_name); // Remove caso já exista
		step_semaphores[i] = sem_open(sem_name, O_CREAT | O_EXCL, 0644, 0);
		if (step_semaphores[i] == SEM_FAILED) {
			perror("sem_open step_semaphore failed");
			exit(1);
		}
	}

	sem_unlink("/main_semaphore");
	main_semaphore = sem_open("/main_semaphore", O_CREAT | O_EXCL, 0644, 0);
	if (main_semaphore == SEM_FAILED) {
		perror("sem_open main_semaphore failed");
		exit(1);
	}
    pthread_t thread_ids[THREAD_NUM];

    //create_threads(thread_ids, DRONE_NUM, TIME_STEPS_NUM, matrix);

	int p[DRONE_NUM]; //PIDs

	for (int i = 0; i < DRONE_NUM; i++) {

		p[i] = fork();

		if (p[i] == -1) {
			perror("Fork Failed");
			exit(11);
		}

		if (p[i] == 0) {

			// child/drone process
            for (int j = 0; j < TIME_STEPS_NUM; j++) {
				sem_wait(step_semaphores[i]); 
				
				run_drone_script(j, matrix, i, MAX_COLLISION_NUM);
				
				sem_post(main_semaphore);
            }
            exit(0);
		}
	}
	
	simulation(TIME_STEPS_NUM, DRONE_NUM);

	//pthread_t thread_ids[THREAD_NUM];
	create_threads(thread_ids, DRONE_NUM, TIME_STEPS_NUM, matrix);

	simulation_finished();                       // let the reporter exit
	for (int i = 0; i < 2; ++i) pthread_join(thread_ids[i], NULL);

	//=================================================================================================================

	//Parent Process
	
    // wait for all the child process to finish
    printf("\nWaiting for drone processes to finish...\n\n");
    for (int i = 0; i < DRONE_NUM; i++) {

     	int status;

       	int finished_pid = waitpid(p[i], &status, 0);

       	if (WIFEXITED(status)) {
       		printf("Drone %d with PID %d ended with value %d\n", i, finished_pid, WEXITSTATUS(status));
       	}
    }
    printf("============================================\n\n");

	// simulation_finished();

	// for (int i = 0; i < THREAD_NUM; ++i)
	// 	pthread_join(thread_ids[i], NULL);
    
    
	for (int i = 0; i < DRONE_NUM; i++) {
		sem_close(step_semaphores[i]);
		char sem_name[64];
		snprintf(sem_name, sizeof(sem_name), "/step_semaphore_%d", i);
		sem_unlink(sem_name);
	}
	sem_close(main_semaphore);
	sem_unlink("/main_semaphore");
    
    //wait for all threads to finish
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    free_position_matrix(matrix, "/matrix_shared_data", DRONE_NUM, TIME_STEPS_NUM);

	printf("Simulation finished with success...\n");

	return 0;
}

