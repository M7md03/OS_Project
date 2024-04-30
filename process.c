#include "headers.h"

int remainingtime;

int main(int agrc, char* argv[]) {
    initClk();

    key_t key = ftok("progfile", 66);           // create unique key
    int msgid = msgget(key, 0666 | IPC_CREAT);  // create message queue and return id

    remainingtime = atoi(argv[1]);
    int pid = atoi(argv[2]);

    struct msgRemaining msg;
    msg.mtype = pid;

    printf("Process %d started with remaining time = %d\n", pid, remainingtime);

    while (remainingtime > 0) {
        int clk = getClk();

        remainingtime--;
        msg.remainingtime = remainingtime;
        msgsnd(msgid, &msg, sizeof(msg), !IPC_NOWAIT);

        while (clk == getClk()) {
        }
    }

    destroyClk(false);

    return 0;
}