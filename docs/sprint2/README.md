# SCOMP

## US261

### Description

- I want to start a simulation process for a figure. So that I can check for collisions before approving it.

### Acceptance Criteria

- This component must be implemented in C and must utilize processes, pipes, and signals.

- The system should fork a new process for each drone in the figure.

- Each drone process should execute its designated movement script.

- Pipes should facilitate communication between the main process and each drone process.

- The main process should track drone positions over time using an appropriate data structure.

### Decisions

- Global variables were defined to control the number of drones for the simulation, the number of time steps and the max
  number of collitions that each drone process can have.

- We decided to use a pipe for each child process, for the communication with the parent process. The drone positions
  are sent from the child processes to the parent process.

- A matrix was defined to track all the drone positions over time.

- We have verifications for the exit status for each drone processes in the parent.

---

## US262

### Description

- As a simulation process, I want to receive movement commands from drone scripts so taht I can track drone positions over time.

### Acceptance criteria

- Each drone process must send position updates to the main process via a pipe.

- The main process should maintain a time-indexed 3D matrix to track drone positions.

- The system must store past positions to anticipate and detect potential collisions.

### Decision

- Each drone writes its position data to a dedicated pipe, and the main simulation process will read from the read end of all drone pipes.

- The 3D matrix (in form of a pointer) will store the position of the drone over time with the dimensions like `time_step`, `drone_id`, `position`.

- With the position for each drone in each time step stored in the 3D matrix, it can be later processed to help detect potential collisions.

---

## US263 - Detect Drone Collitions

### Description

- As a simulation system, I want to continuously monitor drone positions for overlaps so that I can identify and report
  collitions.

### Acceptance Criteria

- The system must detect when two or more drones occupy the same position at the same time.

- Upon detecting a collision, the system should log the event and notify the involved drones via signals.

- Each drone must handle the received signal and notify the system user with a message.

- When a drone receives a `SIGUSR1` (collision detected), it should block other signals while handling it.

- The system should allow early termination if collisions exceed a predefined threshold by sending termination
  signals to drones.

- Drone processes properly handle termination signals and perform any necessary cleanup.

### Decisions

- To verify collitions we will use the 3D matrix that has the positions of the drones in a specific time step.

- Upon executing the drone script, values will be generated and a verification with the matrix will occur.

- Upon detecting a potential collition (same coordinates on x, y or z), the system will notify the user with a message
  and log the collition at a text file.

- The implementation modifies the default behavior of the SIGUSR1 signal, defining a custom handler and blocking other
  signals while the handler is executed. The system also restarts any interrupted system calls.

- To allow early termination upon exceeding a predefined threshold of collitions, we decided to use an array on shared
  memory to store the collision number of each drone process.

- Each time a collision occur, the parent process increments  the number of collisions of that specific drone process
  and verifies if the threshold is exceeded, and if it is the parent will send the SIGINT signal to the drone.

- The drone process upon receiving the SIGINT signal will display a message to the user indicating termination and
  it will exit with the exit status 10, indicating collision threshold exceeded.

---
## US264 - Synchronize drone execution with a time step

### Description

- As a simulation engine, I want to synchronize drone movements based on
  time steps so that I can accurately simulate real-world execution.


### Acceptance Criteria

- The simulation must progress step by step.

- Each drone process should send position updates at defined
  intervals.

- Each drone process should send position updates at defined
  intervals.

### Decisions

- To simulate a real world simulation and synchronize drone movements based on time steps we use a loop.
- In each time step, we send the signal to each drone to continue the execution. We choose the signal 'SIGCONT - continue after stop'.
- After sending the signal, we call us262 to read and save the drone positions.
- Then the us263 to detect and prevent collisions
---

## US265 - Generate a simulation report

### Description

- I want to receive a summary of the simulation results
  So that I can determine if the figure is safe to use.

### Acceptance Criteria

- The system must generate a report and store it in a file.
- The report should include the total number of drones and their execution status.
- If collisions occur, the report must list timestamps and positions.
- The report should indicate whether the figure passed or failed
  validation.

### Decisions

- The report data of the colisions is fetched in the colisions.txt, from the us263.
- The data of the movements of the drones is fetched from the struct created to save them.
- It creates a txt file with the date and time to make each report unique and easier to differentiate.

---