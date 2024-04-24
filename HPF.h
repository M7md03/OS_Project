#include "headers.h"

void swap(struct Process **a, struct Process **b) {
    struct Process *temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(struct Process **arr, int i, int size) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < size && arr[left]->P < arr[smallest]->P) smallest = left;

    if (right < size && arr[right]->P < arr[smallest]->P) smallest = right;

    if (smallest != i) {
        swap(&arr[i], &arr[smallest]);
        minHeapify(arr, smallest, size);
    }
}

void buildMinHeap(struct Process **arr, int size) {
    for (int i = size / 2 - 1; i >= 0; i--) {
        minHeapify(arr, i, size);
    }
}

void insertProcess(struct Process **arr, int *size, struct Process *p) {
    (*size)++;
    arr[*size - 1] = p;
    int i = *size - 1;
    while (i > 0 && arr[(i - 1) / 2]->P > arr[i]->P) {
        swap(&arr[i], &arr[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

struct Process *extractMin(struct Process **arr, int *size) {
    if (*size == 0) return NULL;

    struct Process *min = arr[0];
    arr[0] = arr[*size - 1];
    (*size)--;
    minHeapify(arr, 0, *size);
    return min;
}

void scheduleHPF(struct Process **arr, int size) {
    while (size > 0) {
        struct Process *p = extractMin(arr, &size);
        printf("Process ID: %d, Priority: %d\n", p->ID, p->P);
        DeProcess(p);
    }
}