#ifndef HEADERS_H
#define HEADERS_H

#include <math.h>
#include <signal.h>
#include <stdio.h>  //if you don't use scanf/printf change this include
#include <stdlib.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef short bool;
#define true 1
#define false 0

#define HPF 0
#define SRTN 1
#define RR 2

#define IGNORE_LENGTH 128

#define SHKEY 300

struct Process {
    pid_t pid;  // Process ID
    int ID;
    int ArrivalT;  // Arrival Time
    int StartT;    // Start Time
    int RunT;      // Running Time
    int RemT;      // Remaining Time
    int EndT;      // End Time
    int P;         // Priority
    int MemSize;   // Memory Size
    MemoryNode* MyMemory; 
};

/**
 * @brief Dynamically allocate a process and initialize its attributes.
 *
 * This function allocates memory for a new process and initializes its attributes.
 * It takes the process ID, arrival time, running time, and priority as input parameters.
 * If memory allocation fails, it prints an error message and exits the program.
 * Otherwise, it returns a pointer to the newly created process.
 *
 * @param id The unique identifier for the process.
 * @param at The time at which the process arrives in the system.
 * @param rt The amount of time the process needs to run.
 * @param pr The priority of the process.
 *
 * @return A pointer to the newly created process, or NULL if memory allocation fails.
 */
struct Process *Process(int id, int at, int rt, int pr, int mem) {
    struct Process *p = malloc(sizeof(struct Process));  // Dynamically allocate a process
    if (p == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    p->pid = -20;      // Initalize PID
    p->ID = id;        // Set ID
    p->ArrivalT = at;  // Set Arrival Time
    p->StartT = -1;    // Set Start Time
    p->RunT = rt;      // Set Running Time
    p->RemT = rt;      // Set Remaining Time
    p->P = pr;         // Set Priority
    p->MemSize = mem;  // Set Memory Size
    p->MyMemory = NULL;// no memory allocated yet
    return p;
}

void DeProcess(struct Process *p) {
    free(p);  // Free Memory
}

struct msgRemaining {
    long mtype;
    int remainingtime;
};

struct MinHeap {
    struct Process **array;  // Array of pointers to Process structs
    int size;
    int capacity;
    struct Process *RUN;
};

struct MinBLK {
    struct Process **array;  // Array of pointers to Process structs
    int size;
    int capacity;
};

/**
 * Swaps two elements in an array.
 *
 * @param a a pointer to the first element
 * @param b a pointer to the second element
 */
void swap(struct Process **a, struct Process **b) {
    struct Process *temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Creates a new min heap data structure.
 *
 * @return a pointer to the newly created min heap, or NULL if memory allocation fails
 */
struct MinBLK *MinHeapBLK(int c) {
    struct MinBLK *blk = (struct MinBLK *)malloc(sizeof(struct MinBLK));  // Dynamically allocate a min heap
    blk->capacity = c;                                                    // Initial capacity
    blk->size = 0;
    blk->array = (struct Process **)malloc(
        blk->capacity * sizeof(struct Process *));  // Dynamically allocate memory for the array of pointers
    return blk;
}

void minHeapifyBLK(struct MinBLK *blk, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < blk->size && (blk->array[left]->MemSize < blk->array[smallest]->MemSize ||
                             (blk->array[left]->MemSize == blk->array[smallest]->MemSize &&
                              blk->array[left]->ArrivalT < blk->array[smallest]->ArrivalT)))
        smallest = left;

    if (right < blk->size && (blk->array[right]->MemSize < blk->array[smallest]->MemSize ||
                              (blk->array[right]->MemSize == blk->array[smallest]->MemSize &&
                               blk->array[right]->ArrivalT < blk->array[smallest]->ArrivalT)))
        smallest = right;

    if (smallest != i) {
        swap(&blk->array[i], &blk->array[smallest]);
        minHeapifyBLK(blk, smallest);
    }
}

void insertProcessBLK(struct MinBLK *blk, struct Process *p) {
    int i = blk->size;
    blk->size++;
    blk->array[i] = p;

    // Fix the min heap property if it is violated
    while (i != 0 && (blk->array[(i - 1) / 2]->MemSize > blk->array[i]->MemSize ||
                      (blk->array[(i - 1) / 2]->MemSize == blk->array[i]->MemSize &&
                       blk->array[(i - 1) / 2]->ArrivalT > blk->array[i]->ArrivalT))) {
        swap(&blk->array[i], &blk->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int MinSize(struct MinBLK *blk) {
    if (blk->size == 0) return -1;
    return blk->array[0]->MemSize;
}

struct Process *extractMinBLK(struct MinBLK *blk) {
    if (blk->size == 0) return NULL;
    if (blk->size == 1) {
        blk->size--;
        return blk->array[0];
    }

    // Store the minimum value and remove it from the heap
    struct Process *root = blk->array[0];
    blk->array[0] = blk->array[blk->size - 1];
    blk->size--;
    minHeapifyBLK(blk, 0);

    return root;
}

void FreeBLK(struct MinBLK *blk) {
    free(blk->array);
    free(blk);
}
/**
 * Creates a new min heap data structure.
 *
 * @return a pointer to the newly created min heap, or NULL if memory allocation fails
 */
struct MinHeap *MinHeap(int c) {
    struct MinHeap *minHeap = (struct MinHeap *)malloc(sizeof(struct MinHeap));  // Dynamically allocate a min heap
    minHeap->capacity = c;                                                       // Initial capacity
    minHeap->size = 0;
    minHeap->array = (struct Process **)malloc(
        minHeap->capacity * sizeof(struct Process *));  // Dynamically allocate memory for the array of pointers
    return minHeap;
}

bool isEmptyMin(struct MinHeap *minHeap) { return minHeap->size == 0; }

void FreeMin(struct MinHeap *minHeap) {
    free(minHeap->array);
    free(minHeap);
}

///==============================
// don't mess with this variable//
int *shmaddr;  //
//===============================

int getClk() { return *shmaddr; }

/*
 * All process call this function at the beginning to establish
 * communication between them and the clock module. Again, remember that the
 * clock is only emulation!
 */
void initClk() {
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1) {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of
 * simulation. It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll) {
    shmdt(shmaddr);
    if (terminateAll) {
        killpg(getpgrp(), SIGINT);
    }
}

#endif
