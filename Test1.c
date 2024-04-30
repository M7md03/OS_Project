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
    t->b = 1;
    t->mtype = 6;
    // t->a[0][0] = 3;

    key_t key_id = ftok("keyfile", 65);

    // Create or get the message queue
    int msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    // Define the message buffer
    struct Test message;
    // message.a = t->a;
    message.b = t->b;
    message.mtype = t->mtype;

    // Send the message
    msgsnd(msgq_id, &message, sizeof(message.b), !IPC_NOWAIT);

    // free(t->a[0]);
    // free(t->a);
    free(t);
    return 0;
}