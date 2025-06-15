#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "structs.h"

#define MAX_COLLISIONS 1024

static CollisionEvent collisions[MAX_COLLISIONS];
static int collision_cnt = 0;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cv_work_available = PTHREAD_COND_INITIALIZER;
static int work_left = 1;

static void *collision_thread(void *arg) {

    ThreadCfg cfg = *((ThreadCfg *)arg);
    free(arg);

    write(STDOUT_FILENO, "Collision Thread\n", 17);

    for (int t = 0; t < cfg.steps; ++t) {

        for (int i = 0; i < cfg.drones; ++i) {

            Position pi = cfg.matrix[POS_IDX(t, i, cfg.drones)];

            for (int j = i + 1; j < cfg.drones; ++j) {

                Position pj = cfg.matrix[POS_IDX(t, j, cfg.drones)];

                if (pi.x == pj.x && pi.y == pj.y && pi.z == pj.z) {
                    printf("Collision: D%d & D%d at t=%d (%d,%d,%d)\n",
                           i, j, t, pi.x, pi.y, pi.z);

                    pthread_mutex_lock(&mtx);

                    if (collision_cnt < MAX_COLLISIONS) {
                        collisions[collision_cnt++] = (CollisionEvent){i, j, t, pi};
                    }
                    pthread_cond_signal(&cv_work_available);
                    pthread_mutex_unlock(&mtx);
                }
            }
        }
    }

    // work_left 0, means no more work to do
    pthread_mutex_lock(&mtx);
    work_left = 0;
    pthread_cond_signal(&cv_work_available);
    pthread_mutex_unlock(&mtx);

    return NULL;
}

// Report generation code
static int cfg_total_drones = 0;
static int cfg_total_steps  = 0;

static void *report_thread(void *arg) {

    (void)arg;
    write(STDOUT_FILENO, "Report Thread\n", 14);

    // Wait until collision_thread says there is nothing left to do
    pthread_mutex_lock(&mtx);
    while (work_left || collision_cnt == 0) {
        if (!work_left) break;  // finished & no collisions – we can exit
        pthread_cond_wait(&cv_work_available, &mtx);
    }

    // Build the final report
    time_t rawtime;
    struct tm *timeinfo;
    char timestamp[80];
    char filename[100];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", timeinfo);
    snprintf(filename, sizeof(filename), "simulation_report_%s.txt", timestamp);
    
    FILE *report_file = fopen(filename, "w");
    if (!report_file) {
        perror("Failed to create report file");
        return NULL;
    }

    fprintf(report_file, "=== SIMULATION REPORT ===\n");
    fprintf(report_file, "Total drones        : %d\n", cfg_total_drones);
    fprintf(report_file, "Total time steps    : %d\n", cfg_total_steps);
    fprintf(report_file, "Collisions detected : %d\n\n", collision_cnt);

    for (int k = 0; k < collision_cnt; ++k) {

        CollisionEvent c = collisions[k];
        fprintf(report_file,
                "Collision %2d | drones %d & %d | t=%d "
                "| (%d,%d,%d)\n",
                k + 1, c.drone1, c.drone2, c.time_step,
                c.pos.x, c.pos.y, c.pos.z);
    }

    fprintf(report_file, "\nValidation result : %s\n", collision_cnt ? "FAIL" : "PASS");
    fclose(report_file);

    printf("Final report written to simulation_report_%s.txt\n", timestamp);

    pthread_mutex_unlock(&mtx);
    return NULL;
}

void create_threads(pthread_t tid[2], int drones, int steps, Position *matrix) {

    ThreadCfg *cfg = malloc(sizeof(ThreadCfg));
    *cfg = (ThreadCfg){drones, steps, matrix};

    cfg_total_drones = drones;
    cfg_total_steps  = steps;

    if (pthread_create(&tid[0], NULL, collision_thread, cfg) != 0 ||
        pthread_create(&tid[1], NULL, report_thread,   NULL) != 0) {
        perror("pthread_create");
        exit(2);
    }
}

void simulation_finished(void) {
    pthread_mutex_lock(&mtx);
    work_left = 0;
    pthread_cond_broadcast(&cv_work_available);
    pthread_mutex_unlock(&mtx);
}