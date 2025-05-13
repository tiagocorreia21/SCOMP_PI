#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

void report_generator(Position ***position_Matrix, int num_Drones, int time_Steps) {

    FILE *file = fopen("collitions_logs.txt", "w");
    FILE *relatorio = fopen("Relatorio.txt", "w");

    if (file == NULL) {
        perror("fopen failed");
        exit(1);
    }


    fprintf(relatorio, "\n======= COLISÕES DETETADAS =======\n");

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
            fputs(line, relatorio);
    }

    fprintf(relatorio, "Numero total de drones: %d\n", num_Drones);

    for(int i = 0; i < num_Drones; i++){
        fprintf(relatorio, "=======Drone %d=======\n", i);
        for(int j = 0; j < time_Steps; j++){

        Position position=get_position_3d(position_Matrix, i, j, num_Drones, time_Steps);
        fprintf(relatorio, "Drone %d, Cordenadas -> ( %d, %d, %d ) ", i, position.x, position.y, position.z);

        }

    }





}