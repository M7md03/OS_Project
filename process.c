#include "headers.h"

int remainingtime;

int main(int agrc, char* argv[]) {
    initClk();

    key_t key = ftok("progfile", 66);           // create unique key
    int msgid = msgget(key, 0666 | IPC_CREAT);  // create message queue and return id

    remainingtime = atoi(argv[1]);
    pid_t pid = getpid();
    int id = atoi(argv[2]);

    struct msgRemaining msg;
    msg.mtype = pid;

    printf("%d Process %d started with remaining time = %d\n", pid, id, remainingtime);

    while (remainingtime > 0) {
        int clk = getClk();

        remainingtime--;
        msg.remainingtime = remainingtime;
        printf("Process %d: Remaining time = %d\n", id, remainingtime);
        msgsnd(msgid, &msg, sizeof(msg), !IPC_NOWAIT);
        if (remainingtime == 0) {
            // printf("Procces %d Terminated at clk = %d\n", id, getClk());
            destroyClk(false);
            return 0;
        }
        while (clk == getClk()) {
        }
    }
}