#ifndef RR_H
#define RR_H
#include "headers.h"

struct RoundRobin {
    struct Process **queue;
    int front, rear;
    int capacity;
};

struct RoundRobin *RoundRobin() {
    struct RoundRobin *rr = (struct RoundRobin *)malloc(sizeof(struct RoundRobin));
    rr->capacity = 1;  // Initial capacity
    rr->queue = (struct Process **)malloc(rr->capacity * sizeof(struct Process *));
    rr->front = rr->rear = -1;
    return rr;
}

int isEmpty(struct RoundRobin *rr) { return rr->front == -1; }

void enqueue(struct RoundRobin *rr, struct Process *p) {
    if ((rr->rear + 1) % rr->capacity == rr->front) {
        // Queue is full, resize
        rr->capacity *= 2;
        rr->queue = (struct Process **)realloc(rr->queue, rr->capacity * sizeof(struct Process *));
    }
    if (isEmpty(rr)) {
        rr->front = rr->rear = 0;
    } else {
        rr->rear = (rr->rear + 1) % rr->capacity;
    }
    rr->queue[rr->rear] = p;
}

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

void FreeRoundRobin(struct RoundRobin *rr) {
    free(rr->queue);
    free(rr);
}
#endif