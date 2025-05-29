#ifndef STRUCTS_H
#define STRUCTS_H

#define DRONE_NUM 10

typedef struct {
    int x;
    int y;
    int z;
    int time_step;
    int drone_id;
    int pid;
} Position;

#endif
