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

- To allow early termination upon exceeding a predefined threshold of collitions, we decided to use a control variable 
   which stores the number of collitions in each drone.

- The system will check this variable and every time a potential collition occurs and terminate if this 
   value is greater than the defined threshold. The system will also send a termination signal to the drone that exceeded
    the threshold.

- Upon receiving the termination signal, the drones will perform any necessary cleanup and exit. 

---

## US264

---

## US265

---