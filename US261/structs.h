#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    int x;
    int y;
    int z;
} Position;

typedef struct {
    Position current_position;
    int fd[2];
    int pid;
    int active;
}

#endif
