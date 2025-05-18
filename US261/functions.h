#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

void capture_drone_movement(int fd, Position *pos);

int process_movement(Position *current_pos, Position *new_pos);

void store_position(Position ***positions_ptr, int drone_id, int time_step, Position pos);

void print_positions(Position ***positions_ptr, int drone_count, int time_step);

void run_drone_script(int write_fd, int time_step_num, Position ***position_matrix, int drone_id, int time_steps, int start_log_line_count);

void initialize_drone_positions(Position ***positions_matrix, int time_step_num, int drone_num);

Position*** allocate_position_matrix(int num_drones, int time_steps);

void free_position_matrix(Position*** matrix, int num_drones, int time_steps);

Position get_position_3d(Position*** matrix, int drone_id, int time_step, int num_drones, int time_steps_num);

Position get_position(Position *positions_ptr, int drone_id, int time_step);

Position generate_position(Position ***position_matrix, int drone_id, int time_step);

shared_data_type *allocate_shared_memory(char *shm_name);

void deallocate_shared_memory(char *shm_name, shared_data_type *shared_data);

#endif
