#ifndef STRUCTS_H
#define STRUCTS_H

#define DRONE_NUM 2

typedef struct {
    int x;
    int y;
    int z;
    int time_step;
    int drone_id;
    int pid;
} Position;

//macro to access position [t][d]
#define POS_IDX(time_step,drone_id, drone_num) ((time_step) * (drone_num) + (drone_num))

#endif
