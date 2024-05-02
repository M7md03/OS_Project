#ifndef SRTN_H
#define SRTN_H
#include "headers.h"

/**
 * Fixes the min heap property of an element at a specific index.
 *
 * @param minHeap a pointer to the min heap
 * @param i the index of the element to fix
 */
void minHeapifySRTN(struct MinHeap* minHeap, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < minHeap->size && (minHeap->array[left]->RemT < minHeap->array[smallest]->RemT ||
                                 (minHeap->array[left]->RemT == minHeap->array[smallest]->RemT &&
                                  minHeap->array[left]->ArrivalT < minHeap->array[smallest]->ArrivalT))) {
        smallest = left;
    }
    if (right < minHeap->size && (minHeap->array[right]->RemT < minHeap->array[smallest]->RemT ||
                                  (minHeap->array[right]->RemT == minHeap->array[smallest]->RemT &&
                                   minHeap->array[right]->ArrivalT < minHeap->array[smallest]->ArrivalT))) {
        smallest = right;
    }
    if (smallest != i) {
        swap(&minHeap->array[i], &minHeap->array[smallest]);
        minHeapifySRTN(minHeap, smallest);
    }
}

/**
 * Inserts a new process into a min heap.
 *
 * @param minHeap a pointer to the min heap
 * @param p a pointer to the process to insert
 */
void insertProcessSRTN(struct MinHeap* minHeap, struct Process* p) {
    if (minHeap->size == minHeap->capacity) {
        minHeap->capacity *= 2;  // Double the capacity
        minHeap->array = (struct Process**)realloc(
            minHeap->array, minHeap->capacity * sizeof(struct Process*));  // Resize the array of pointers
    }
    int i = minHeap->size;
    minHeap->size++;
    minHeap->array[i] = p;

    // Fix the min heap property if it is violated
    while (i != 0 && (minHeap->array[(i - 1) / 2]->RemT > minHeap->array[i]->RemT ||
                      (minHeap->array[(i - 1) / 2]->RemT == minHeap->array[i]->RemT &&
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
struct Process* extractMinSRTN(struct MinHeap* minHeap) {
    if (minHeap->size == 0) return NULL;
    if (minHeap->size == 1) {
        minHeap->size--;
        return minHeap->array[0];
    }

    // Store the minimum value and remove it from the heap
    struct Process* root = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    minHeapifySRTN(minHeap, 0);

    return root;
}

int MinTime(struct MinHeap* minHeap) {
    if (minHeap->size == 0) return -1;
    return minHeap->array[0]->RemT;
}

void SRTNScheduling(int ProcNum) {
    key_t key_id = ftok("keyfile", 65);

    int msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    int rec_val;

    key_t key = ftok("progfile", 66);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msgRemaining msg;

    struct Process* Proc = (struct Process*)malloc(ProcNum * sizeof(struct Process));
    struct MinHeap* minHeap = MinHeap(ProcNum);
    int g = 0;
    while (ProcNum > 0) {
        int clk = getClk();
        if (clk == 0) {
            continue;
        }
        bool flag = true;
        while (flag) {
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
                    char* args[] = {"./process.out", run_str, id_str, NULL};
                    if (execv(args[0], args) == -1) {
                        perror("Error in execv");
                    }
                }
                Proc[g].pid = pid;
                printf("#%d  Process %d Recieved, ArivT: %d, RunT: %d, P: %d, PID: %d\n", clk, Proc[g].ID,
                       Proc[g].ArrivalT, Proc[g].RunT, Proc[g].P, Proc[g].pid);

                // Update the process ID and enqueue it
                kill(pid, SIGSTOP);
                insertProcessSRTN(minHeap, &Proc[g]);
                g++;
            } else {
                flag = false;
            }
        }
        if (!isEmptyMin(minHeap) && minHeap->RUN == NULL) {
            // Extract a process and set it to the RUN state
            minHeap->RUN = extractMinSRTN(minHeap);
            printf("Proccess %d is in RUN\n", minHeap->RUN->ID);
        }
        if (minHeap->RUN != NULL) {
            kill(minHeap->RUN->pid, SIGCONT);
            // Update the start time if necessary
            if (minHeap->RUN->StartT == -1) {
                minHeap->RUN->StartT = clk;
            }
            msgrcv(msgid, &msg, sizeof(msg), 0, !IPC_NOWAIT);
            minHeap->RUN->RemT = msg.remainingtime;
            // Check if the process is finished
            if (minHeap->RUN->RemT > MinTime(minHeap) || minHeap->RUN->RemT == 0) {
                if (minHeap->RUN->RemT > 0) {
                    // Time quantum is exhausted, pause the process and enqueue it
                    if (!isEmptyMin(minHeap)) {
                        kill(minHeap->RUN->pid, SIGSTOP);
                        insertProcessSRTN(minHeap, minHeap->RUN);
                        minHeap->RUN = NULL;
                    }
                } else {
                    // Process is finished, pause the process, update end time, and free memory
                    wait(NULL);
                    minHeap->RUN->EndT = clk + 1;
                    ProcNum--;
                    printf("Proccess %d is Finished, StartT = %d, EndT = %d\n", minHeap->RUN->ID, minHeap->RUN->StartT,
                           minHeap->RUN->EndT);
                    minHeap->RUN = NULL;
                }
            }
            // Wait until the clock time changes
            while (clk == getClk()) {
            }
        }
    }
    // Free the allocated memory
    FreeMin(minHeap);
}
#endif