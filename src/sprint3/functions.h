#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

void capture_drone_movement(int fd, Position *pos);

void store_position(Position ***positions_ptr, int drone_id, int time_step, Position pos);

void print_positions(Position ***positions_ptr, int drone_count, int time_step);

void run_drone_script(int time_step_num, Position ***position_matrix, int drone_id, int time_steps);

void initialize_drone_positions(Position ***positions_matrix, int time_step_num, int drone_num);

Position*** allocate_position_matrix(int num_drones, int time_steps);

void free_position_matrix(Position*** matrix, int num_drones, int time_steps);

Position generate_position(Position ***position_matrix, int drone_id, int time_step);

#endif
