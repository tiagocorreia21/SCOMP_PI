#ifndef STRUCTS_H
#define STRUCTS_H

#define DRONE_NUM 10

//macro to access position [t][d]
#define POS_IDX(time_step,drone_id, drone_num) ((time_step) * (drone_num) + (drone_id))

typedef struct {
    int x;
    int y;
    int z;
    int time_step;
    int drone_id;
    int pid;
} Position;

typedef struct {
    int drone_num;
    int time_step_num;
    Position *matrix;
} collition_thread_info;

typedef struct {
    int drone1;
    int drone2;
    int time_step;
    Position pos;
} CollisionEvent;

typedef struct {
    int drones;
    int steps;
    Position *matrix;
} ThreadCfg;

#endif
