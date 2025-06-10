#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

void run_drone_script(int time_step, Position *matrix, int drone_id, int max_collisions_num);

Position* allocate_shared_position_matrix(int num_drones, int time_steps, char shm_name[]);

void free_position_matrix(Position *matrix, char shm_name[], int drone_num, int time_step_num);

Position get_position(Position* matrix, int drone_id, int time_step, int drone_num);

Position generate_position(Position *matrix, int time_step, int drone_id);

void print_position(Position *matrix, int drone_id, int time_step, int drone_num);

#endif
