#ifndef HPF_H
#define HPF_H
#include "headers.h"

/**
 * Performs the min-heapify operation on a given MinHeap at a specified index.
 *
 * @param minHeap The MinHeap structure to perform the min-heapify operation on.
 * @param i The index of the element to start the min-heapify operation from.
 */
void minHeapifyHPF(struct MinHeap *minHeap, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < minHeap->size && (minHeap->array[left]->P < minHeap->array[smallest]->P ||
                                 (minHeap->array[left]->P == minHeap->array[smallest]->P &&
                                  minHeap->array[left]->ArrivalT < minHeap->array[smallest]->ArrivalT)))
        smallest = left;

    if (right < minHeap->size && (minHeap->array[right]->P < minHeap->array[smallest]->P ||
                                  (minHeap->array[right]->P == minHeap->array[smallest]->P &&
                                   minHeap->array[right]->ArrivalT < minHeap->array[smallest]->ArrivalT)))
        smallest = right;

    if (smallest != i) {
        swap(&minHeap->array[i], &minHeap->array[smallest]);
        minHeapifyHPF(minHeap, smallest);
    }
}

/**
 * Inserts a new process into a min heap.
 *
 * @param minHeap a pointer to the min heap
 * @param p a pointer to the process to insert
 */
void insertProcessHPF(struct MinHeap *minHeap, struct Process *p) {
    int i = minHeap->size;
    minHeap->size++;
    minHeap->array[i] = p;

    // Fix the min heap property if it is violated
    while (i != 0 && (minHeap->array[(i - 1) / 2]->P > minHeap->array[i]->P ||
                      (minHeap->array[(i - 1) / 2]->P == minHeap->array[i]->P &&
                       minHeap->array[(i - 1) / 2]->ArrivalT > minHeap->array[i]->ArrivalT))) {
        swap(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

/**
 * Extracts and returns the minimum process from a min heap.
 *
 * @param minHeap a pointer to the min heap
 * @return a pointer to the minimum process, or NULL if the min heap is empty
 */
struct Process *extractMinHPF(struct MinHeap *minHeap) {
    if (minHeap->size == 0) return NULL;
    if (minHeap->size == 1) {
        minHeap->size--;
        return minHeap->array[0];
    }

    // Store the minimum value and remove it from the heap
    struct Process *root = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    minHeapifyHPF(minHeap, 0);

    return root;
}

void HPFScheduling(int ProcNum, FILE *fptr, float *totalWTA, int *totalWait, int *totalUtil, float *WTA) {
    struct MinBLK *BLK = (struct MinBLK *)malloc(ProcNum * sizeof(struct MinBLK));

    key_t key_id = ftok("keyfile", 65);

    int msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    int rec_val;

    key_t key = ftok("progfile", 66);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msgRemaining msg;

    struct Process *Proc = (struct Process *)malloc(ProcNum * sizeof(struct Process));
    struct MinHeap *minHeap = MinHeap(ProcNum);
    int g = 0;
    int i = 0;
    while (ProcNum > 0) {
        int clk = getClk();
        while (1) {
            struct Process messagegen;
            struct timespec req;
            req.tv_sec = 0;
            req.tv_nsec = 1;  // 1 nanosecond

            nanosleep(&req, NULL);
            rec_val = msgrcv(msgq_id, &messagegen, sizeof(messagegen), 0, IPC_NOWAIT);

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
                kill(pid, SIGSTOP);
                insertProcessHPF(minHeap, &Proc[g]);
                g++;
            } else {
                break;
            }
        }
        if (minHeap->RUN != NULL) {
            msg.mtype = minHeap->RUN->pid;
            // Receive the remaining time of the running process
            msgrcv(msgid, &msg, sizeof(msg), msg.mtype, !IPC_NOWAIT);
            minHeap->RUN->RemT = msg.remainingtime;
            // Check if the process is finished
            if (minHeap->RUN->RemT == 0) {
                // Process is finished, pause the process, update end time, and free memory
                wait(NULL);
                ProcNum--;
                printf(
                    "At time\t%d\tprocess\t%d\tfinished arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%.2f\n",
                    clk, minHeap->RUN->ID, minHeap->RUN->ArrivalT, minHeap->RUN->RunT, minHeap->RUN->RemT,
                    clk - minHeap->RUN->ArrivalT - minHeap->RUN->RunT + minHeap->RUN->RemT,
                    clk - minHeap->RUN->ArrivalT, (float)(clk - minHeap->RUN->ArrivalT) / minHeap->RUN->RunT);
                fprintf(
                    fptr,
                    "At time\t%d\tprocess\t%d\tfinished arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%.2f\n",
                    clk, minHeap->RUN->ID, minHeap->RUN->ArrivalT, minHeap->RUN->RunT, minHeap->RUN->RemT,
                    clk - minHeap->RUN->ArrivalT - minHeap->RUN->RunT + minHeap->RUN->RemT,
                    clk - minHeap->RUN->ArrivalT, (float)(clk - minHeap->RUN->ArrivalT) / minHeap->RUN->RunT);
                *totalWTA += (float)(clk - minHeap->RUN->ArrivalT) / minHeap->RUN->RunT;
                *totalWait += clk - minHeap->RUN->ArrivalT - minHeap->RUN->RunT + minHeap->RUN->RemT;
                WTA[i] = (float)(clk - minHeap->RUN->ArrivalT) / minHeap->RUN->RunT;
                i++;
                minHeap->RUN = NULL;
            }
        }
        if (!isEmptyMin(minHeap) && minHeap->RUN == NULL) {
            // Extract a process and set it to the RUN state
            minHeap->RUN = extractMinHPF(minHeap);
            // Resume the process execution
            kill(minHeap->RUN->pid, SIGCONT);
            // Update the start time if necessary
            if (minHeap->RUN->StartT == -1) {
                minHeap->RUN->StartT = clk;
                printf("At time\t%d\tprocess\t%d\tstarted  arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", clk,
                       minHeap->RUN->ID, minHeap->RUN->ArrivalT, minHeap->RUN->RunT, minHeap->RUN->RemT,
                       clk - minHeap->RUN->ArrivalT - minHeap->RUN->RunT + minHeap->RUN->RemT);
                fprintf(fptr, "At time\t%d\tprocess\t%d\tstarted  arr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", clk,
                        minHeap->RUN->ID, minHeap->RUN->ArrivalT, minHeap->RUN->RunT, minHeap->RUN->RemT,
                        clk - minHeap->RUN->ArrivalT - minHeap->RUN->RunT + minHeap->RUN->RemT);
            }
        }
        if (minHeap->RUN != NULL) {
            (*totalUtil)++;
            // printf("totalutil = %d\n", *totalUtil);
        }
        // Wait until the clock time changes
        while (clk == getClk()) {
        }
    }
    // Free the allocated memory
    FreeMin(minHeap);
    free(Proc);
    free(BLK);
}

#endif