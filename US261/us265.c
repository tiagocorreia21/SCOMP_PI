#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "structs.h"
#include "functions.h"

void report_generator(Position ***position_Matrix, int num_Drones, int time_Steps, int start_log_line_count) {

	FILE *file = fopen("collitions_logs.txt", "r");

    char filename[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(filename, sizeof(filename), "Relatorio_%Y-%m-%d_%H-%M-%S.txt", t);

    FILE *relatorio = fopen(filename, "w");
    
    if (file == NULL || relatorio == NULL) {
        perror("fopen failed");
        if (file != NULL) fclose(file);
			if (relatorio != NULL) fclose(relatorio);
				exit(1);
    }


    fprintf(relatorio, "\n======= COLISÕES DETETADAS =======\n");
    
    char line[256];
    int line_num = 0;
    
	while (line_num < start_log_line_count && fgets(line, sizeof(line), file) != NULL) {
		line_num++;
	}	

	// Agora copiar só as linhas a partir daqui
	while (fgets(line, sizeof(line), file) != NULL) {
		fputs(line, relatorio);
	}

    fprintf(relatorio, "\nNumero total de drones: %d\n\n", num_Drones);

    for(int i = 0; i < num_Drones; i++){
        fprintf(relatorio, "=======Drone %d=======\n", i);
        for(int j = 0; j < time_Steps; j++){
			
			Position position=get_position_3d(position_Matrix, i, j, num_Drones, time_Steps);
			fprintf(relatorio, "Cordenadas -> ( %d, %d, %d )\n ", position.x, position.y, position.z);
			
        }

    }


	fclose(file);
    fclose(relatorio);

    printf("Relatório salvo em: %s\n", filename);


}
