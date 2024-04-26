#ifndef HEADERS_H
#define HEADERS_H

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
#include <unistd.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

struct Process {
    int ID;
    int ArrivalT;  // Arrival Time
    int StartT;    // Start Time
    int RunT;      // Running Time
    int RemT;      // Remaining Time
    int EndT;      // End Time
    int P;         // Priority
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
struct Process *Process(int id, int at, int rt, int pr) {
    struct Process *p = malloc(sizeof(struct Process));  // Dynamically allocate a process
    if (p == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    p->ID = id;        // Set ID
    p->ArrivalT = at;  // Set Arrival Time
    p->RunT = rt;      // Set Running Time
    p->RemT = rt;      // Set Remaining Time
    p->P = pr;         // Set Prioroty
    return p;
}

void DeProcess(struct Process *p) {
    free(p);  // Free Memory
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