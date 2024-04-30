#include "headers.h"

struct Test {
    // int **a;
    int b;
    int mtype;
};

int main() {
    struct Test *t = (struct Test *)malloc(sizeof(struct Test));
    // t->a = (int **)malloc(sizeof(int *));
    // t->a[0] = (int *)malloc(sizeof(int));

    key_t key_id = ftok("keyfile", 65);

    // Create or get the message queue
    int msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    int rec_val;

    // Define the message buffer
    struct Test messagerec;
    printf("Before\n");
    // Recieve the message
    msgrcv(msgq_id, &messagerec, sizeof(messagerec.b), 0, !IPC_NOWAIT);
    printf("After\n");
    t->b = messagerec.b;

    printf("b: %d\n", t->b);

    struct msqid_ds ctrl_status_ds;
    msgctl(msgq_id, IPC_RMID, 0);

    // free(t->a[0]);
    // free(t->a);
    free(t);
    return 0;
}