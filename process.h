#ifndef PROCESS_H
#define PROCESS_H
#include "headers.h"
#include <stdlib.h>
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
struct Process* CreateProcess(int id, int at, int rt, int pr) 
{
    struct Process *p = malloc(sizeof(struct Process));  // Dynamically allocate a process
    if (p == NULL) 
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    p->pid = -20;      // Initalize PID
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
#endif