void run_drone_script(int write_fd, int drone_num);
void initialize_drone_positions(Position *drone_position, int time_step_num, int drone_num);
void run_simulation(int pids[], int fd[][2], Position *positions_ptr, int drone_count, int time_steps, shared_data_type *shared_data);
