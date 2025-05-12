#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

// US262 functions
void capture_drone_movement(int fd, Position *pos);
int process_movement(Position *current_pos, Position *new_pos);
void store_position(Position *positions_ptr, int drone_id, int time_step, Position pos);
void print_positions(Position *positions_ptr, int drone_count, int time_step);

void run_drone_script(int write_fd, int drone_num);
void initialize_drone_positions(Position *drone_position, int time_step_num, int drone_num);
void run_simulation(int pids[], int fd[][2], Position *positions_ptr, int drone_count, int time_steps, shared_data_type *shared_data);
shared_data_type *allocate_shared_memory(char *shm_name);
Position*** allocate_position_matrix(int num_drones, int time_steps);
void free_position_matrix(Position*** matrix, int num_drones, int time_steps);
Position get_position_3d(Position*** matrix, int drone_id, int time_step, int num_drones, int time_steps_num);
Position get_position(Position *positions_ptr, int drone_id, int time_step);

#endif
