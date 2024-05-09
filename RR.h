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
 * @brief Creates a new RoundRobin instance with the given time quantum and capacity.
 * @param q The time quantum for each process
 * @param capacity The capacity of the queue
 * @return A pointer to the newly created RoundRobin instance, or NULL if memory allocation fails
 */
struct RoundRobin *createRoundRobin(int q, int capacity) {
    struct RoundRobin *rr = (struct RoundRobin *)malloc(sizeof(struct RoundRobin));
    if (rr == NULL) {
        return NULL;
    }
    rr->capacity = capacity;
    rr->queue = (struct Process **)malloc(rr->capacity * sizeof(struct Process *));
    if (rr->queue == NULL) {
        free(rr);
        return NULL;
    }
    rr->front = rr->rear = -1;
    rr->quantum = q;
    rr->runQuantum = q;
    rr->RUN = NULL;
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
        // Queue is full
        return 0;
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
void RoundRobinScheduling(int q, int ProcNum, FILE *fptr, float *totalWTA, int *totalWait, int *totalUtil, float *WTA, FILE *memlog) 
{
    struct MinBLK *BLK = (struct MinBLK *)malloc(ProcNum * sizeof(struct MinBLK));

    struct MemoryNode *root = createMemoryNode(MaxSize, NULL, 0);

    struct Process *Proc = (struct Process *)malloc(ProcNum * sizeof(struct Process));
    // Create a RoundRobin struct
    struct RoundRobin *rr = createRoundRobin(q, ProcNum);

    // Get the key for the message queue
    key_t key_id = ftok("keyfile", 65);

    // Create or get the message queue
    int msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    int rec_val;

    // Get the key for the remaining time message queue
    key_t key = ftok("progfile", 66);

    // Create or get the remaining time message queue
    int msgid = msgget(key, 0666 | IPC_CREAT);

    // Define the remaining time message struct
    struct msgRemaining msg;
    int g = 0;
    int i = 0;
    // Start the scheduling loop
    while (ProcNum > 0) {
        // Get the current clock time
        int clk = getClk();
        while (1) {
            // Receive messages from the message queue
            struct Process messagegen;
            struct timespec req;
            req.tv_sec = 0;
            req.tv_nsec = 1;  // 1 nanosecond

            nanosleep(&req, NULL);
            rec_val = msgrcv(msgq_id, &messagegen, sizeof(messagegen), 0, IPC_NOWAIT);
            // Check for receive errors
            if (rec_val != -1) {
                // Fork a child process
                Proc[g] = messagegen;
                pid_t pid = fork();
                // Check for fork errors
                if (pid == -1) {
                    perror("Error in fork");
                    exit(-1);
                } else if (pid == 0) {
                    // Child process
                    char run_str[10], pid_str[10], id_str[10];
                    sprintf(run_str, "%d", Proc[g].RunT);
                    sprintf(id_str, "%d", Proc[g].ID);
                    char *args[] = {"./process.out", run_str, id_str, NULL};
                    if (execv(args[0], args) == -1) {
                        perror("Error in execv");
                    }
                }
                Proc[g].pid = pid;
                // Update the process ID and enqueue it
                kill(Proc[g].pid, SIGSTOP);
                Proc[g].MyMemory = allocate(Proc[g].MemSize, root);
                if (Proc[g].MyMemory == NULL) {
                    insertProcessBLK(BLK, &Proc[g]);
                } 
                else 
                {   //printing in memory.log the allocated memory
                    printf(
                        "At time\t%d\tallocated\t%d\tbytes for process\t%d\tfrom\t%d\tto\t%d\t",
                        clk, Proc[g].MemSize ,Proc[g].ID, Proc[g].MyMemory->block.start_address, 
                        Proc[g].MyMemory->block.end_address );
                    printf(
                        memlog,
                        "At time\t%d\tallocated\t%d\tbytes for process\t%d\tfrom\t%d\tto\t%d\t",
                        clk, Proc[g].MemSize ,Proc[g].ID, Proc[g].MyMemory->block.start_address, 
                        Proc[g].MyMemory->block.end_address );
                    enqueue(rr, &Proc[g]);
                }
                g++;
            } else {
                break;
            }
        }
        // Resume the process execution and receive the remaining time
        if (rr->RUN != NULL) {
            msg.mtype = rr->RUN->pid;
            msgrcv(msgid, &msg, sizeof(msg), msg.mtype, !IPC_NOWAIT);
            rr->RUN->RemT = msg.remainingtime;
            rr->runQuantum--;
            // Check if the time quantum is exhausted or the process is finished
            if (rr->runQuantum == 0 || rr->RUN->RemT == 0) {
                if (rr->RUN->RemT > 0) {
                    // Time quantum is exhausted, pause the process and enqueue it
                    if (!isEmpty(rr)) {
                        kill(rr->RUN->pid, SIGSTOP);
                        printf("At time\t%d\tprocess\t%d\tstopped  arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", clk,
                               rr->RUN->ID, rr->RUN->ArrivalT, rr->RUN->RunT, rr->RUN->RemT,
                               clk - rr->RUN->ArrivalT - rr->RUN->RunT + rr->RUN->RemT);
                        fprintf(fptr, "At time\t%d\tprocess\t%d\tstopped  arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n",
                                clk, rr->RUN->ID, rr->RUN->ArrivalT, rr->RUN->RunT, rr->RUN->RemT,
                                clk - rr->RUN->ArrivalT - rr->RUN->RunT + rr->RUN->RemT);
                        enqueue(rr, rr->RUN);
                        rr->RUN = NULL;
                    }
                } else {
                    // Process is finished, pause the process, update end time, and free memory
                    wait(NULL);
                    ProcNum--;
                    printf(
                        "At time\t%d\tprocess\t%d\tfinished "
                        "arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%.2f\n",
                        clk, rr->RUN->ID, rr->RUN->ArrivalT, rr->RUN->RunT, rr->RUN->RemT,
                        clk - rr->RUN->ArrivalT - rr->RUN->RunT, clk - rr->RUN->ArrivalT,
                        (float)(clk - rr->RUN->ArrivalT) / rr->RUN->RunT);
                    fprintf(fptr,
                            "At time\t%d\tprocess\t%d\tfinished "
                            "arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%.2f\n",
                            clk, rr->RUN->ID, rr->RUN->ArrivalT, rr->RUN->RunT, rr->RUN->RemT,
                            clk - rr->RUN->ArrivalT - rr->RUN->RunT, clk - rr->RUN->ArrivalT,
                            (float)(clk - rr->RUN->ArrivalT) / rr->RUN->RunT);
                    *totalWTA += (float)(clk - rr->RUN->ArrivalT) / rr->RUN->RunT;
                    *totalWait += clk - rr->RUN->ArrivalT - rr->RUN->RunT;
                    WTA[i] = (float)(clk - rr->RUN->ArrivalT) / rr->RUN->RunT;
                    i++;
                    //printing in memory.log the freed memory
                    printf(
                    "At time\t%d\tfreed\t%d\tbytes from process\t%d\tfrom\t%d\tto\t%d\n",
                    clk, rr->RUN->MemSize ,rr->RUN->ID, rr->RUN->MyMemory->block.start_address, 
                    rr->RUN->MyMemory->block.end_address);
                    fprintf(
                        memlog,
                        "At time\t%d\tfreed\t%d\tbytes from process\t%d\tfrom\t%d\tto\t%d\n",
                        clk, rr->RUN->MemSize ,rr->RUN->ID, rr->RUN->MyMemory->block.start_address, 
                        rr->RUN->MyMemory->block.end_address);
                    deallocateMemory(rr->RUN->MyMemory);
                    rr->RUN = NULL;
                }
                rr->runQuantum = rr->quantum;
            }
        }
        if (!isEmpty(rr) && rr->RUN == NULL) {
            // Dequeue a process and set it to the RUN state
            rr->RUN = dequeue(rr);
            kill(rr->RUN->pid, SIGCONT);
            if (rr->RUN->StartT != -1) {
                printf("At time\t%d\tprocess\t%d\tresumed  arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", clk,
                       rr->RUN->ID, rr->RUN->ArrivalT, rr->RUN->RunT, rr->RUN->RemT,
                       clk - rr->RUN->ArrivalT - rr->RUN->RunT + rr->RUN->RemT);
                fprintf(fptr, "At time\t%d\tprocess\t%d\tresumed  arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", clk,
                        rr->RUN->ID, rr->RUN->ArrivalT, rr->RUN->RunT, rr->RUN->RemT,
                        clk - rr->RUN->ArrivalT - rr->RUN->RunT + rr->RUN->RemT);
            }
            // Update the start time if necessary
            if (rr->RUN->StartT == -1) {
                rr->RUN->StartT = clk;
                printf("At time\t%d\tprocess\t%d\tstarted  arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", clk,
                       rr->RUN->ID, rr->RUN->ArrivalT, rr->RUN->RunT, rr->RUN->RemT,
                       clk - rr->RUN->ArrivalT - rr->RUN->RunT + rr->RUN->RemT);
                fprintf(fptr, "At time\t%d\tprocess\t%d\tstarted  arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", clk,
                        rr->RUN->ID, rr->RUN->ArrivalT, rr->RUN->RunT, rr->RUN->RemT,
                        clk - rr->RUN->ArrivalT - rr->RUN->RunT + rr->RUN->RemT);
            }
        }
        if (rr->RUN != NULL) {
            (*totalUtil)++;
            // printf("totalutil = %d\n", *totalUtil);
        }
        struct Process *p;
        if (!isEmptyBLK(BLK)) {
            p = extractMinBLK(BLK);
            p->MyMemory = allocate(p->MemSize, root);
            if (p->MyMemory != NULL) 
            {
                //printing in memory.log the allocated memory
                printf(
                    "At time\t%d\tallocated\t%d\tbytes for process\t%d\tfrom\t%d\tto\t%d\n",
                    clk, p->MemSize ,p->ID, p->MyMemory->block.start_address, 
                    p->MyMemory->block.end_address );
                fprintf(
                    memlog,
                    "At time\t%d\tallocated\t%d\tbytes for process\t%d\tfrom\t%d\tto\t%d\n",
                    clk, p->MemSize ,p->ID, p->MyMemory->block.start_address, 
                    p->MyMemory->block.end_address ); 
                enqueue(rr, p);
            } else {
                insertProcessBLK(BLK, p);
            }
        }
        // Wait until the clock time changes
        while (clk == getClk()) {
        }
    }
    // Free the allocated memory
    FreeRoundRobin(rr);
    free(Proc);
    free(BLK);
}
#endif