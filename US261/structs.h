#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    int x;
    int y;
    int z;
    int time_step;
} Position;

typedef struct {
    Position current_position;
    int fd[2];
    int pid;
    int active;
} DroneInfo;

typedef struct {
    Position drone_positions[10];
} TimeStep;

typedef struct {
    int collition_num;
} shared_data_type;

#endif
