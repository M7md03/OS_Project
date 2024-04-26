#ifndef HPF_H
#define HPF_H
#include "headers.h"

struct MinHeap {
    struct Process** array;  // Array of pointers to Process structs
    int size;
    int capacity;
};

/**
 * Creates a new min heap data structure.
 *
 * @return a pointer to the newly created min heap, or NULL if memory allocation fails
 */
struct MinHeap* MinHeap() {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));  // Dynamically allocate a min heap
    minHeap->capacity = 2;                                                      // Initial capacity
    minHeap->size = 0;
    minHeap->array = (struct Process**)malloc(
        minHeap->capacity * sizeof(struct Process*));  // Dynamically allocate memory for the array of pointers
    return minHeap;
}

/**
 * Swaps two elements in an array.
 *
 * @param a a pointer to the first element
 * @param b a pointer to the second element
 */
void swap(struct Process** a, struct Process** b) {
    struct Process* temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Fixes the min heap property of an element at a specific index.
 *
 * @param minHeap a pointer to the min heap
 * @param i the index of the element to fix
 */
void minHeapify(struct MinHeap* minHeap, int i) {
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
        minHeapify(minHeap, smallest);
    }
}

/**
 * Inserts a new process into a min heap.
 *
 * @param minHeap a pointer to the min heap
 * @param p a pointer to the process to insert
 */
void insertProcess(struct MinHeap* minHeap, struct Process* p) {
    if (minHeap->size == minHeap->capacity) {
        minHeap->capacity *= 2;  // Double the capacity
        minHeap->array = (struct Process**)realloc(
            minHeap->array, minHeap->capacity * sizeof(struct Process*));  // Resize the array of pointers
    }
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
struct Process* extractMin(struct MinHeap* minHeap) {
    if (minHeap->size == 0) return NULL;
    if (minHeap->size == 1) {
        minHeap->size--;
        return minHeap->array[0];
    }

    // Store the minimum value and remove it from the heap
    struct Process* root = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    minHeapify(minHeap, 0);

    return root;
}

void FreeMin(struct MinHeap* minHeap) {
    free(minHeap->array);
    free(minHeap);
}
#endif