#ifndef HPF_H
#define HPF_H
#include "headers.h"

/**
 * Performs the min-heapify operation on a given MinHeap at a specified index.
 *
 * @param minHeap The MinHeap structure to perform the min-heapify operation on.
 * @param i The index of the element to start the min-heapify operation from.
 */
void minHeapifyHPF(struct MinHeap* minHeap, int i) {
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
void insertProcessHPF(struct MinHeap* minHeap, struct Process* p) {
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
struct Process* extractMinHPF(struct MinHeap* minHeap) {
    if (minHeap->size == 0) return NULL;
    if (minHeap->size == 1) {
        minHeap->size--;
        return minHeap->array[0];
    }

    // Store the minimum value and remove it from the heap
    struct Process* root = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    minHeapifyHPF(minHeap, 0);

    return root;
}

void HPFScheduling(int ProcNum) {
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

        bool flag = true;
        while (flag) {
            struct Process messagegen;
            usleep(1);
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
                insertProcessHPF(minHeap, &Proc[g]);
                g++;
            } else {
                flag = false;
            }
        }
        if (!isEmptyMin(minHeap) && minHeap->RUN == NULL) {
            // Extract a process and set it to the RUN state
            minHeap->RUN = extractMinHPF(minHeap);
            printf("Proccess %d is in RUN\n", minHeap->RUN->ID);
        }
        if (minHeap->RUN != NULL) {
            kill(minHeap->RUN->pid, SIGCONT);
            // Update the start time if necessary
            if (minHeap->RUN->StartT == -1) {
                minHeap->RUN->StartT = getClk();
            }
            msgrcv(msgid, &msg, sizeof(msg), 0, !IPC_NOWAIT);
            minHeap->RUN->RemT = msg.remainingtime;
            // Check if the process is finished
            if (minHeap->RUN->RemT == 0) {
                // Process is finished, pause the process, update end time, and free memory
                wait(NULL);
                minHeap->RUN->EndT = clk;
                ProcNum--;
                printf("Proccess %d is Finished, StartT = %d\n", minHeap->RUN->ID, minHeap->RUN->StartT);
                minHeap->RUN = NULL;
            }
        }
        // Wait until the clock time changes
        while (clk == getClk()) {
        }
    }
    // Free the allocated memory
    FreeMin(minHeap);
}

#endif