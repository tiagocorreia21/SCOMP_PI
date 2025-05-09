#include <signal.h>
#include <string.h>

void handle_collition_sigurs1(int signo) {

    write(STDOUT_FILENO, "\nCollition Detected\n", 20);
}

void verify_collitions() {

    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));

    act.sa_handler = handle_collition_sigurs1

    // restard interrupted system calls
    act.sa_flags = SA_RESTART;

    // block all other signals
    sigfillset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL);

    // TODO: Implement the collition verification
}