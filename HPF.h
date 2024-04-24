#include "headers.h"

/**
 * Swaps two Process pointers.
 *
 * This function takes two pointers to Process pointer variables and swaps their
 * values. It is used to rearrange processes in a heap data structure without
 * copying the entire Process structure, thus improving efficiency.
 *
 * @param a A pointer to the first Process pointer to be swapped.
 * @param b A pointer to the second Process pointer to be swapped.
 */
void swap(struct Process **a, struct Process **b) {
    struct Process *temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Maintains the min heap property for a given subtree.
 *
 * This function ensures that the subtree rooted at index `i` in the array `arr`
 * of Process pointers satisfies the min heap property. If the current node
 * violates the min heap property (i.e., it is larger than either of its
 * children), the function recursively adjusts the subtree by swapping the
 * current node with its smallest child and then calling itself on the location
 * of the smallest child.
 *
 * @param arr An array of pointers to Process structures representing the heap.
 * @param i The index of the current node in the array `arr` that is being
 * heapified.
 * @param size The total number of elements in the heap.
 */
void minHeapify(struct Process **arr, int i, int size) {
    int smallest = i;       // Initialize the smallest as root
    int left = 2 * i + 1;   // Calculate index of left child
    int right = 2 * i + 2;  // Calculate index of right child

    // If left child is smaller than root
    if (left < size && arr[left]->P < arr[smallest]->P) smallest = left;

    // If right child is smaller than smallest so far
    if (right < size && arr[right]->P < arr[smallest]->P) smallest = right;

    // If smallest is not root
    if (smallest != i) {
        // Swap the current node with its smallest child
        swap(&arr[i], &arr[smallest]);
        // Recursively heapify the affected sub-tree
        minHeapify(arr, smallest, size);
    }
}

/**
 * Builds a min heap from an unsorted array of Process pointers.
 *
 * This function iteratively applies the minHeapify function to all non-leaf
 * nodes of the array, starting from the last non-leaf node all the way up to
 * the root. This ensures that the heap property is maintained for every node,
 * effectively converting an unsorted array into a min heap. A min heap is a
 * complete binary tree where the value of each node is less than or equal to
 * the values of its children.
 *
 * @param arr An array of pointers to Process structures that will be organized
 * into a min heap.
 * @param size The number of elements in the array `arr`.
 */
void buildMinHeap(struct Process **arr, int size) {
    for (int i = size / 2 - 1; i >= 0; i--) {
        minHeapify(arr, i, size);
    }
}

/**
 * Inserts a new Process into the min heap.
 *
 * This function adds a new Process to the heap and then ensures the min heap
 * property is maintained by percolating the new element up the heap until it is
 * in the correct position. The heap size is incremented to accommodate the new
 * element, and the element is inserted at the end of the heap array. The
 * function then adjusts the heap by swapping the new element with its parent
 * until the min heap property is restored.
 *
 * @param arr A pointer to the array of Process pointers representing the heap.
 * @param size A pointer to an integer representing the current size of the
 * heap. This value is incremented to reflect the addition of the new Process.
 * @param p A pointer to the Process to be inserted into the heap.
 */
void insertProcess(struct Process **arr, int *size, struct Process *p) {
    (*size)++;  // Increment the size of the heap to accommodate the new element
    arr[*size - 1] = p;  // Insert the new element at the end of the heap
    int i = *size - 1;   // Index of the newly inserted element
    // Percolate up: Swap the new element with its parent until the min heap
    // property is restored
    while (i > 0 && arr[(i - 1) / 2]->P > arr[i]->P) {
        swap(&arr[i], &arr[(i - 1) / 2]);
        i = (i - 1) / 2;  // Move to the parent index
    }
}

/**
 * Extracts the minimum Process from the min heap and removes it.
 *
 * This function extracts the Process with the smallest priority from the min
 * heap and removes it. It then restores the min heap property by percolating
 * the remaining elements up the heap. The function returns a pointer to the
 * extracted Process. If the heap is empty, it returns NULL.
 *
 * @param arr A pointer to the array of Process pointers representing the heap.
 * @param size A pointer to an integer representing the current size of the
 * heap. This value is decremented to reflect the removal of the extracted
 * Process.
 * @return A pointer to the Process with the smallest priority, or NULL if the
 * heap is empty.
 */
struct Process *extractMin(struct Process **arr, int *size) {
    if (*size == 0) return NULL;

    struct Process *min = arr[0];
    arr[0] = arr[*size - 1];
    (*size)--;
    minHeapify(arr, 0, *size);
    return min;
}

/**
 * Schedules the High Priority First (HPF) algorithm on the given array of
 * Process pointers.
 *
 * This function iteratively extracts the Process with the smallest priority
 * from the min heap and removes it, then prints its details and deallocates the
 * memory for the Process. The function continues this process until the heap is
 * empty.
 *
 * @param arr A pointer to the array of Process pointers representing the heap.
 * @param size A pointer to an integer representing the current size of the
 * heap. This value is decremented to reflect the removal of the extracted
 * Process.
 */
void scheduleHPF(struct Process **arr, int *size) {
    while (*size > 0) {
        struct Process *p = extractMin(arr, size);
        printf("Process ID: %d, Priority: %d\n", p->ID, p->P);
        DeProcess(p);
    }
}