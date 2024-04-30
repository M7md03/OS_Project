#include "headers.h"

void clearResources(int);
int msgq_id;

int main(int argc, char* argv[]) {
    signal(SIGINT, clearResources);
    key_t key_id = ftok("keyfile", 65);
    int QUANTA = 0;
    int ALGORITHM;
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum) {
    // TODO Clears all resources in case of interruption
    printf("process generator stopped\n");
    int msgq_del;
    msgq_del = msgctl(msgq_id, IPC_RMID, 0);  // destroy message queue used to communicate with scheduler
    destroyClk(true);                         // destroy the clk
    exit(0);
}
