#include "structs.h"

extern sem_t* step_semaphores[];
extern sem_t* main_semaphore;

void simulation(int time_steps, int drone_count);
