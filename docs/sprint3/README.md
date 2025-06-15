# SCOMP
- If you need to change the number of timesteps and the number of max colitions,you should change the correspondent variables in main.c, structs.h, us364.c, and thread_functions.c files. 
## US361

### Description

Initialize hybrid simulation Environment With Shared Memory

- As a system user, I want to start the simulation with a multi-threaded parent process and multiple child drone 
  processes communicating through a shared memory area, So that the system efficiently coordinates simulation 
  data across processes.

### Acceptance Criteria

- The parent process spawns dedicated threads for its functionalities;
- Each drone is launched as an independent process;
- A shared memory segment is allocated and properly initialized for inter-process communication;
- Drone processes are configured to use semaphores for synchronization.

### Decisions

- There are global variables on the main.c file to control the number of drones and time steps of the simulation;
- A function for the allocation of the shared data (matrix) was implemented;
- A function to deallocate the matrix was implemented;
- A new process is created for each drone and each drone has a script that generates positions and writes it on the shared matrix;
- Threads are created for collition scanning and report generation.

---

## US362

### Description

Implement Function-Specific Threads in the Parent Process

- As a simulation controller, I want the parent process to have at least two dedicated threads (one for collision detection, one for report generation);
- So that each functionality operates concurrently and independently.

### Acceptance criteria

- The parent process creates a collision detection thread responsible for scanning the shared memory for drone position conflicts;
- A report generation thread is created to compile simulation results and respond to collision events;
- Any additional thread that you deem appropriate for any of the required functionalities;
- Threads are managed using mutexes and condition variables for internal synchronization.

### Decisions

- A thread for collition detection is created;
- A thread for report generation is created.

---

## US363 - Detect Drone Collitions

### Description
- Detects and handles collisions between drones during the simulation.
- As a simulation controller, I want the system to continuously monitor
drone positions across all timesteps to detect spatial collisions, 
so that any conflicts are identified and can be reported.

### Acceptance Criteria

- The system must detect when two or more drones occupy the same positon at the same time.
- Upon detecting a collision, the system should log the event and notify the involved drones via signals.
- Each drone must handle the received signal and notify the system user with a message.
- When a drone receives a SIGUSR1 (collision detected), it should block other signals while handling it.
- The system should allow early termination if collisions exceed a predefined threshold by sending termination signals to drones.

### Decisions

- A separate thread was implemented for collision detection.
- The thread iterates through the shared memory matrix and compares drone positions pairwise for each timestep;
- When a collision is found, it sets a shared flag and signals a condition variable to wake the report thread;

---
## US364 - Synchronize drone execution with a time step

### Description

- As a simulation engine i want to synchronize the simulation's step-by-step progression using
semaphores, so that all drone processes and parent threads advance in lockstep
through each simulation time step.


### Acceptance Criteria

- Semaphores are used to control the progression of each
simulation time step.

### Decisions

- Two semaphores used
- One main semaphore called main_semaphore, where the drone signals the main process that it's finished
- One second semaphore called step_semaphores, where the main process authorize the drones to proceed. This semaphore beeing an array with the size of the number of drones.

---

## US365 - Generate a simulation report

### Description

Generates and stores the final simulation Report

- As a system user, I want a comprehensive report that details the simulation outcomes — including drone execution statuses, collision events (with timestamps and positions), and overall validation results — So that I can assess the safety and performance of the figure post-simulation.

### Acceptance Criteria

- The report generation thread aggregates data from the shared memory once the simulation concludes;
- The report includes the total number of drones, individual execution statuses, and detailed collision events;
- The final validation result (pass/fail) is clearly indicated;
- The complete report is saved to a file for future reference.

### Decisions

- The report generation thread is tasked with assembling and writting the final report upon simulation completion;
- The report will be saved in a txt file with the name **"simulation_report_YYYY_MMM_dd_HH_mm_ss.txt"** for acessibility;
- A status *"Pass/Faill"* will be printed at the end of the report for an at-a-glance result.

---