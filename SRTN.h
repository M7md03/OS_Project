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

#endif