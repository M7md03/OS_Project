#include "HPF.h"

int main() {
    struct Process *arr[100];  // Assuming a maximum of 100 processes
    int size = 0;

    // Create some processes and insert them into the min-heap
    insertProcess(arr, &size, Process(1, 0, 5, 2));
    insertProcess(arr, &size, Process(2, 1, 3, 9));
    insertProcess(arr, &size, Process(3, 2, 7, 3));
    insertProcess(arr, &size, Process(3, 2, 7, 5));
    insertProcess(arr, &size, Process(3, 2, 7, 1));

    // Build the min-heap
    buildMinHeap(arr, size);

    // Schedule processes using HPF algorithm
    scheduleHPF(arr, size);

    return 0;
}