void run_drone_script(int write_fd, int drone_num);
void initialize_drone_positions(Position *drone_position, int time_step_num, int drone_num);
shared_data_type *allocate_shared_memory(char *shm_name);
