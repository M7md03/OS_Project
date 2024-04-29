#ifndef RR_H
#define RR_H

#include "headers.h"

/**
 * @file RR.h
 * @brief This file contains the implementation of the Round Robin scheduling algorithm.
 */

/**
 * @struct RoundRobin
 * @brief Represents the Round Robin scheduling algorithm.
 */
struct RoundRobin {
    struct Process **queue; /**< The queue of processes */
    int front;              /**< The front index of the queue */
    int rear;               /**< The rear index of the queue */
    int capacity;           /**< The capacity of the queue */
    int quantum;            /**< The time quantum for each process */
    struct Process *RUN;    /**< The process that is currently running */
    int runQuantum;         /**< The remaining quantum for the running process */
};

/**
 * @brief Creates a new RoundRobin instance with the given time quantum.
 * @param q The time quantum for each process
 * @return A pointer to the newly created RoundRobin instance, or NULL if memory allocation fails
 */
struct RoundRobin *createRoundRobin(int q) {
    struct RoundRobin *rr = (struct RoundRobin *)malloc(sizeof(struct RoundRobin));
    if (rr == NULL) {
        return NULL;
    }
    rr->capacity = 1;  // Initial capacity
    rr->queue = (struct Process **)malloc(rr->capacity * sizeof(struct Process *));
    if (rr->queue == NULL) {
        free(rr);
        return NULL;
    }
    rr->front = rr->rear = -1;
    rr->quantum = q;
    rr->runQuantum = q;
    return rr;
}

/**
 * @brief Checks if the RoundRobin instance is empty.
 * @param rr The RoundRobin instance
 * @return 1 if the RoundRobin instance is empty, 0 otherwise
 */
bool isEmpty(struct RoundRobin *rr) { return rr->front == -1; }

/**
 * @brief Enqueues a process into the RoundRobin instance.
 * @param rr The RoundRobin instance
 * @param p The process to enqueue
 * @return 1 if the process is successfully enqueued, 0 otherwise
 */
bool enqueue(struct RoundRobin *rr, struct Process *p) {
    if ((rr->rear + 1) % rr->capacity == rr->front) {
        // Queue is full, resize
        rr->capacity *= 2;
        struct Process **newQueue = (struct Process **)realloc(rr->queue, rr->capacity * sizeof(struct Process *));
        if (newQueue == NULL) {
            return 0;
        }
        rr->queue = newQueue;
    }
    if (isEmpty(rr)) {
        rr->front = rr->rear = 0;
    } else {
        rr->rear = (rr->rear + 1) % rr->capacity;
    }
    rr->queue[rr->rear] = p;
    return 1;
}

/**
 * @brief Dequeues a process from the RoundRobin instance.
 * @param rr The RoundRobin instance
 * @return The dequeued process, or NULL if the RoundRobin instance is empty
 */
struct Process *dequeue(struct RoundRobin *rr) {
    if (isEmpty(rr)) {
        printf("Queue is empty\n");
        return NULL;
    }
    struct Process *p = rr->queue[rr->front];
    if (rr->front == rr->rear) {
        rr->front = rr->rear = -1;
    } else {
        rr->front = (rr->front + 1) % rr->capacity;
    }
    return p;
}

/**
 * @brief Frees the memory allocated for the RoundRobin instance.
 * @param rr The RoundRobin instance
 */
void FreeRoundRobin(struct RoundRobin *rr) {
    free(rr->queue);
    free(rr);
}

void RoundRobinScheduling(int q) {
    struct RoundRobin *rr = createRoundRobin(q);
    key_t key_id;
    int msgq_id;
    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    int rec_val;
    if (msgq_id == -1) {
        perror("Error in create");
        exit(-1);
    }
    struct msgbuff message;
    while (true) {
        int clk = getClk();
        rec_val = msgrcv(msgq_id, &message, sizeof(message), 0, !IPC_NOWAIT);
        if (rec_val == -1) {
            perror("Error in receive");
            break;
        } else {
            if (message.size == 0) {
                break;
            }
            for (int i = 0; i < message.size; i++) {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("Error in fork");
                    exit(-1);
                } else if (pid == 0) {
                    // Child process
                    char quantum_str[10];
                    sprintf(quantum_str, "%d", rr->quantum);
                    char *args[] = {"./process.out", quantum_str, NULL};
                    execv(args[0], args);
                    perror("Error in execv");
                    exit(-1);
                }
                message.p[i]->pid = pid;
                kill(pid, SIGSTOP);
                enqueue(rr, message.p[i]);
            }
            break;
        }
        if (!isEmpty(rr)) {
            rr->RUN = dequeue(rr);
            if (rr->RUN->StartT == -1) {
                rr->RUN->StartT = clk;
            }
        }
        while (clk == getClk()) {
        }
    }
}
#endif