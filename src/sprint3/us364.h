#include "structs.h"

extern sem_t* step_semaphores[];
extern sem_t* main_semaphore;

void simulation(Position *matrix, int time_steps, int drone_count, int max_collition_num);
