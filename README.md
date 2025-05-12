# SCOMP

## US261

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

- Upon detecting a collition, the system will notify the user with a message and log the collition at a text file.

- The implementation modifies the default behavior of the SIGUSR1 signal, defining a custom handler and blocking other 
  signals while the handler is executed. The system also restarts any interrupted system calls.

- To allow early termination upon exceeding a predefined threshold of collitions, we decided to use a control variable 
   at a shared memory segment. The system will check this variable every time a collition occurs and terminate if this 
   value is greater than the threshold. The system will also send a termination signal to all drones.

- Upon receiving the termination signal, the drones will perform any necessary cleanup and exit. 

---

## US264

---

## US265

---