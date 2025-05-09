#include "../US261/structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h> // For select
#include <string.h>

#define NUM_DRONES 3

int main() {
    int pipe_fds[NUM_DRONES][2]; // Array of pipe file descriptors [drone_idx][read/write]
    pid_t drone_pids[NUM_DRONES];
    int drone_read_fds[NUM_DRONES]; // Array of read ends for select

    for (int i = 0; i < NUM_DRONES; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        drone_pids[i] = fork();

        if (drone_pids[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (drone_pids[i] == 0) { // Child process (Drone)
            // Close the read end the drone doesn't need
            close(pipe_fds[i][0]);

            // Close the read/write ends of pipes for other drones (important!)
            for (int j = 0; j < NUM_DRONES; j++) {
                if (i != j) {
                    close(pipe_fds[j][0]);
                    close(pipe_fds[j][1]);
                }
            }

            // Execute the drone script logic (replace with your actual drone code)
            printf("Drone %d starting...\n", i);
            // In a real scenario, you'd pass the write_fd to the drone's main logic
            // For this example, we'll just close it to show the concept
            close(pipe_fds[i][1]); // Drone closes its write end when done

            exit(EXIT_SUCCESS); // Drone process exits
        } else { // Parent process (Main)
            // Close the write end the parent doesn't need
            close(pipe_fds[i][1]);
            // Store the read end for reading from this drone
            drone_read_fds[i] = pipe_fds[i][0];
        }
    }

    // Main process continues here...
    // Now you would use select or poll with drone_read_fds to read data
    // ... simulation loop using select/poll ...

    // Clean up (wait for children, close remaining fds)
    for (int i = 0; i < NUM_DRONES; i++) {
        waitpid(drone_pids[i], NULL, 0);
        close(drone_read_fds[i]);
    }

    printf("Main process finished.\n");

    return 0;
}
