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

/**
 * @file RR.h
 * @brief Contains the implementation of the Round Robin scheduling algorithm.
 */

/**
 * @brief Implements the Round Robin scheduling algorithm.
 *
 * This function performs the Round Robin scheduling algorithm to schedule processes.
 * It takes the time quantum and the number of processes as input parameters.
 *
 * @param q The time quantum for each process.
 * @param ProcNum The number of processes to be scheduled.
 */
void RoundRobinScheduling(int q, int ProcNum) {
    struct Process *Proc = (struct Process *)malloc(ProcNum * sizeof(struct Process));
    // Create a RoundRobin struct
    struct RoundRobin *rr = createRoundRobin(q);

    // Get the key for the message queue
    key_t key_id = ftok("keyfile", 65);

    // Create or get the message queue
    int msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    int rec_val;

    // Define the message buffer
    struct msgbuff messagegen;

    // Get the key for the remaining time message queue
    key_t key = ftok("progfile", 66);

    // Create or get the remaining time message queue
    int msgid = msgget(key, 0666 | IPC_CREAT);

    // Define the remaining time message struct
    struct msgRemaining msg;
    int g = 0;
    int flag = 0;
    // Start the scheduling loop
    while (true) {
        // Get the current clock time
        int clk = getClk();

        // Check if there are processes to be scheduled
        if (ProcNum > 0) {
            // Receive messages from the message queue
            rec_val = msgrcv(msgq_id, &messagegen, sizeof(messagegen.p), 0, IPC_NOWAIT);
            Proc[g] = messagegen.p;
            // Check for receive errors
            if (rec_val == -1) {
                perror("Error in receive");
            } else {
                // Fork a child process
                pid_t pid = fork();

                // Check for fork errors
                if (pid == -1) {
                    perror("Error in fork");
                    exit(-1);
                } else if (pid == 0) {
                    // Child process
                    char run_str[10], pid_str[10];
                    sprintf(run_str, "%d", Proc[g].RunT);
                    sprintf(pid_str, "%d", Proc[g].pid);
                    char *args[] = {"./process.out", run_str, pid_str, NULL};
                    execv(args[0], args);
                    perror("Error in execv");
                    exit(-1);
                }

                // Update the process ID and enqueue it
                Proc[g].pid = pid;
                kill(pid, SIGSTOP);
                enqueue(rr, &Proc[g]);
                g++;
                ProcNum--;
            }
        }

        // Check if the Round Robin queue is not empty
        if (!isEmpty(rr)) {
            // Dequeue a process and set it to the RUN state
            rr->RUN = dequeue(rr);
            printf("Proccess %d is in RUN\n", rr->RUN->ID);

            // Update the start time if necessary
            if (rr->RUN->StartT == -1) {
                rr->RUN->StartT = clk;
            }

            // Resume the process execution and receive the remaining time
            kill(rr->RUN->pid, SIGCONT);
            msgrcv(msgid, &msg, sizeof(msg), 0, !IPC_NOWAIT);
            rr->RUN->RemT = msg.remainingtime;
            rr->runQuantum--;
        }

        // Check if the time quantum is exhausted or the process is finished
        if (rr->runQuantum == 0 || rr->RUN->RemT == 0) {
            if (rr->RUN->RemT > 0) {
                // Time quantum is exhausted, pause the process and enqueue it
                rr->runQuantum = rr->quantum;
                kill(rr->RUN->pid, SIGSTOP);
                enqueue(rr, rr->RUN);
                rr->RUN = NULL;
            } else {
                // Process is finished, pause the process, update end time, and free memory
                rr->runQuantum = rr->quantum;
                kill(rr->RUN->pid, SIGSTOP);
                rr->RUN->EndT = clk;
                printf("Process %d Ended\n", rr->RUN->ID);
                rr->RUN = NULL;
            }
        }
        if (ProcNum == 0 && isEmpty(rr) && rr->RUN == NULL) {
            FreeRoundRobin(rr);
            break;
        }
        // Wait until the clock time changes
        while (clk == getClk()) {
        }
    }
    // Destroy the remaining time message queue
    msgctl(msgid, IPC_RMID, NULL);

    // Destroy the main message queue
    msgctl(msgq_id, IPC_RMID, NULL);
}
#endif