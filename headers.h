#ifndef HEADERS_H
#define HEADERS_H

#include <math.h>
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
#include <time.h>
#include <unistd.h>

typedef short bool;
#define true 1
#define false 0

#define HPF 0
#define SRTN 1
#define RR 2

#define MaxSize 1024

#define IGNORE_LENGTH 128

#define SHKEY 300

typedef struct MemoryBlock  // Struct to represent a memory block
{
    int start_address;
    int end_address;
} MemoryBlock;

typedef struct MemoryNode  // Struct to represent a node in the  tree
{
    int size;  // size of the node
    int used;  // check if used or free
    MemoryBlock block;
    struct MemoryNode *left_child;
    struct MemoryNode *right_child;
    struct MemoryNode *parent;
} MemoryNode;

struct Process {
    pid_t pid;  // Process ID
    int ID;
    int ArrivalT;  // Arrival Time
    int StartT;    // Start Time
    int RunT;      // Running Time
    int RemT;      // Remaining Time
    int EndT;      // End Time
    int P;         // Priority
    int MemSize;   // Memory Size
    MemoryNode *MyMemory;
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
struct Process *Process(int id, int at, int rt, int pr, int mem) {
    struct Process *p = malloc(sizeof(struct Process));  // Dynamically allocate a process
    if (p == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    p->pid = -20;        // Initalize PID
    p->ID = id;          // Set ID
    p->ArrivalT = at;    // Set Arrival Time
    p->StartT = -1;      // Set Start Time
    p->RunT = rt;        // Set Running Time
    p->RemT = rt;        // Set Remaining Time
    p->P = pr;           // Set Priority
    p->MemSize = mem;    // Set Memory Size
    p->MyMemory = NULL;  // no memory allocated yet
    return p;
}

void DeProcess(struct Process *p) {
    free(p);  // Free Memory
}

struct msgRemaining {
    long mtype;
    int remainingtime;
};

struct MinHeap {
    struct Process **array;  // Array of pointers to Process structs
    int size;
    int capacity;
    struct Process *RUN;
};

/**
 * Swaps two elements in an array.
 *
 * @param a a pointer to the first element
 * @param b a pointer to the second element
 */
void swap(struct Process **a, struct Process **b) {
    struct Process *temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapifyBLK(struct MinHeap *blk, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < blk->size && (blk->array[left]->MemSize < blk->array[smallest]->MemSize ||
                             (blk->array[left]->MemSize == blk->array[smallest]->MemSize &&
                              blk->array[left]->ArrivalT < blk->array[smallest]->ArrivalT)))
        smallest = left;

    if (right < blk->size && (blk->array[right]->MemSize < blk->array[smallest]->MemSize ||
                              (blk->array[right]->MemSize == blk->array[smallest]->MemSize &&
                               blk->array[right]->ArrivalT < blk->array[smallest]->ArrivalT)))
        smallest = right;

    if (smallest != i) {
        swap(&blk->array[i], &blk->array[smallest]);
        minHeapifyBLK(blk, smallest);
    }
}

void insertProcessBLK(struct MinHeap *blk, struct Process *p) {
    int i = blk->size;
    blk->size++;
    blk->array[i] = p;
    // Fix the min heap property if it is violated
    while (i != 0 && (blk->array[(i - 1) / 2]->MemSize > blk->array[i]->MemSize ||
                      (blk->array[(i - 1) / 2]->MemSize == blk->array[i]->MemSize &&
                       blk->array[(i - 1) / 2]->ArrivalT > blk->array[i]->ArrivalT))) {
        swap(&blk->array[i], &blk->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

struct Process *extractMinBLK(struct MinHeap *blk) {
    if (blk->size == 0) return NULL;
    if (blk->size == 1) {
        blk->size--;
        return blk->array[0];
    }

    // Store the minimum value and remove it from the heap
    struct Process *root = blk->array[0];
    blk->array[0] = blk->array[blk->size - 1];
    blk->size--;
    minHeapifyBLK(blk, 0);

    return root;
}

int MinSize(struct MinHeap *blk) {
    if (blk->size == 0) return -1;
    return blk->array[0]->MemSize;
}

void FreeBLK(struct MinHeap *blk) {
    free(blk->array);
    free(blk);
}
/**
 * Creates a new min heap data structure.
 *
 * @return a pointer to the newly created min heap, or NULL if memory allocation fails
 */
struct MinHeap *MinHeap(int c) {
    struct MinHeap *minHeap = (struct MinHeap *)malloc(sizeof(struct MinHeap));  // Dynamically allocate a min heap
    minHeap->capacity = c;                                                       // Initial capacity
    minHeap->size = 0;
    minHeap->array = (struct Process **)malloc(
        minHeap->capacity * sizeof(struct Process *));  // Dynamically allocate memory for the array of pointers
    return minHeap;
}

bool isEmptyMin(struct MinHeap *minHeap) { return minHeap->size == 0; }

void FreeMin(struct MinHeap *minHeap) {
    free(minHeap->array);
    free(minHeap);
}

struct MemoryNode *createMemoryNode(int size, struct MemoryNode *parent, int start_address) {
    struct MemoryNode *node = (struct MemoryNode *)malloc(sizeof(struct MemoryNode));
    node->size = size;
    node->block.start_address = start_address;
    node->block.end_address = start_address + size - 1;
    node->left_child = NULL;
    node->right_child = NULL;
    node->parent = parent;
    node->used = 0;
    return node;
}
MemoryNode *findBestFitNodeOfSize(MemoryNode *root, int size) {
    if (root == NULL) {
        return NULL;  // If tree is empty, return NULL
    }

    MemoryNode *bestFit = NULL;  // Initialize the best fit node to NULL

    // Traverse the tree in preorder
    if (root->left_child != NULL) {
        bestFit = findBestFitNodeOfSize(root->left_child, size);
    }

    // Check if the current node is a leaf node
    if (root->left_child == NULL && root->right_child == NULL) {
        // If the leaf node has the required size and is free
        if (root->size >= size && root->used == 0 && (root->size) / 2 < size) {
            // If bestFit is NULL or the current node's size is smaller than the bestFit's size
            if (bestFit == NULL || root->size < bestFit->size) {
                bestFit = root;  // Update bestFit to the current node
            }
        }
    }

    if (root->right_child != NULL) {
        MemoryNode *rightBestFit = findBestFitNodeOfSize(root->right_child, size);
        if (rightBestFit != NULL && (bestFit == NULL || rightBestFit->size < bestFit->size)) {
            bestFit = rightBestFit;
        }
    }
    return bestFit;  // Return the best fit node found
}

// takes a process and alocates it in memory if there is space else it return false (no space)
struct MemoryNode *allocate(int Psize, struct MemoryNode *root) {
    if (root == NULL) return NULL;
    MemoryNode *Bestfit = findBestFitNodeOfSize(root, Psize);
    if (Bestfit != NULL) {
        Bestfit->used = 1;
    } else {
        if (root->used == 0) {
            int num = root->size;
            int half = num / 2;
            if (Psize <= half)  // don't place in root
            {
                if (root->left_child == NULL && root->right_child == NULL)  // if root has no children
                {
                    // create children
                    root->left_child = createMemoryNode(half, root, root->block.start_address);
                    root->right_child = createMemoryNode(half, root, (root->left_child->block.end_address) + 1);
                    // printf("creating left and right children\n");
                    if (half / 2 == Psize) {
                        root->left_child->used = 1;
                        // printf("Allocating in created left child\n");
                        return root->left_child;
                    } else {
                        MemoryNode *left_result = allocate(Psize, root->left_child);
                        if (left_result != NULL) {
                            left_result->used = 1;
                            return left_result;
                        }
                    }
                } else {
                    MemoryNode *left_result = allocate(Psize, root->left_child);
                    if (left_result != NULL) {
                        left_result->used = 1;
                        // printf("Allocated in existing LEFT child \n");
                        return left_result;
                    } else {
                        MemoryNode *right_result = allocate(Psize, root->right_child);
                        if (right_result != NULL) {
                            // printf("Allocated in existing RIGHT child \n");
                            right_result->used = 1;
                            return right_result;
                        }
                    }
                }
            } else {
                if (root->left_child == NULL && root->right_child == NULL) {
                    root->used = 1;
                    return root;
                } else {
                    return NULL;
                }
            }
        } else {
            return NULL;
        }
    }
}

void deallocateMemory(struct MemoryNode *node)  // Function to deallocate memory
{
    if (node == NULL) {
        return;
    }

    if (node->left_child == NULL && node->right_child == NULL) {
        // deallocate memory and recursively update parent nodes
        struct MemoryNode *parent = node->parent;
        if (parent != NULL)  // root of the tree no where to go
        {
            if (parent->left_child == node) {
                parent->left_child->used = 0;
                // printf("FREEING MEMORY FROM %d to %d \n", parent->left_child->block.start_address,
                //        parent->left_child->block.end_address);
                if (parent->right_child != NULL)  // right child is free
                {
                    if (parent->right_child->left_child == NULL && parent->right_child->right_child == NULL &&
                        parent->right_child->used == 0) {
                        // printf("FREEING MEMORY FROM %d to %d \n", parent->right_child->block.start_address,
                        //        parent->right_child->block.end_address);
                        free(parent->right_child);
                        free(parent->left_child);
                        parent->right_child = NULL;
                        parent->left_child = NULL;
                        deallocateMemory(parent);  // go deallocate the parent to merge back memory if needed
                    }
                }
                return;
            } else if (parent->right_child == node) {
                parent->right_child->used = 0;  // the node is the right child
                // printf("FREEING MEMORY FROM %d to %d \n", parent->right_child->block.start_address,
                //        parent->right_child->block.end_address);
                if (parent->left_child != NULL)  // left child is free
                {
                    if (parent->left_child->left_child == NULL && parent->left_child->right_child == NULL &&
                        parent->left_child->used == 0) {
                        // printf("FREEING MEMORY FROM %d to %d \n", parent->left_child->block.start_address,
                        //        parent->left_child->block.end_address);
                        free(parent->left_child);
                        free(parent->right_child);
                        parent->left_child = NULL;
                        parent->right_child = NULL;
                        deallocateMemory(parent);  // go deallocate the parent to merge back memory if needed
                    }
                }
                return;
            }
        } else {
            // the node is the root of the tree
            return;
        }
    }
    // if node has children
    if (node->left_child->used == 0 && node->right_child->used == 0) {
        free(node->left_child);
        free(node->right_child);
        node->left_child = NULL;
        node->right_child = NULL;
        deallocateMemory(node->parent);
    }
    return;
}

// preorder traversal of the tree prints the leaves
void PreorderTraverse(MemoryNode *root) {
    if (root == NULL) return;
    PreorderTraverse(root->left_child);
    PreorderTraverse(root->right_child);

    if (root->left_child == NULL && root->right_child == NULL)
        printf("block_size:%d start:%d end:%d used: %d \n ", root->size, root->block.start_address,
               root->block.end_address, root->used);
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
