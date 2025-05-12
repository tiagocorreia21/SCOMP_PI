#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    double x;
    double y;
    double z;
    int time_step;
} Position;

typedef struct {
    Position current_position;
    int fd[2];
    int pid;
    int active;
} DroneInfo;

typedef struct {
    int collition_num;
} shared_data_type;

#endif
