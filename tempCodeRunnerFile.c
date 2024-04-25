 // Sample usage
    struct MinHeap* minHeap = MinHeap();

    // Insert processes into the min heap
    insertProcess(minHeap, Process(1, 0, 5, 1));
    insertProcess(minHeap, Process(2, 2, 4, 2));
    insertProcess(minHeap, Process(3, 5, 3, 3));
    insertProcess(minHeap, Process(4, 4, 3, 4));
    insertProcess(minHeap, Process(5, 8, 3, 1));
    insertProcess(minHeap, Process(6, 2, 3, 4));

    // Remove processes from the min heap based on priority
    while (minHeap->size != 0) {
        struct Process* p = extractMin(minHeap);
        printf("Process ID: %d, Priority: %d, AT: %d\n", p->ID, p->P, p->ArrivalT);
        DeProcess(p);
    }

    // Free memory
    FreeMin(minHeap);