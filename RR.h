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
};

/**
 * @brief Creates a new RoundRobin instance with the given time quantum.
 * @param q The time quantum for each process
 * @return A pointer to the newly created RoundRobin instance, or NULL if memory allocation fails
 */
struct RoundRobin *RoundRobin(int q) {
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
    return rr;
}

/**
 * @brief Checks if the RoundRobin instance is empty.
 * @param rr The RoundRobin instance
 * @return 1 if the RoundRobin instance is empty, 0 otherwise
 */
int isEmpty(struct RoundRobin *rr) { return rr->front == -1; }

/**
 * @brief Enqueues a process into the RoundRobin instance.
 * @param rr The RoundRobin instance
 * @param p The process to enqueue
 * @return 1 if the process is successfully enqueued, 0 otherwise
 */
int enqueue(struct RoundRobin *rr, struct Process *p) {
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

#endif