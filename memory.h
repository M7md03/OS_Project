#ifndef MEMORY_H
#define MEMORY_H
#include "headers.h"
#include <math.h>

#define MaxSize 1024

typedef struct MemoryBlock                // Struct to represent a memory block
{                
    int start_address;
    int end_address;
} 
MemoryBlock;

typedef struct MemoryNode                  // Struct to represent a node in the  tree
 {
    int size;                             //size of the node
    int used;                            //check if used or free
    MemoryBlock block;
    struct MemoryNode* left_child;
    struct MemoryNode* right_child;
    struct MemoryNode* parent;
} 
MemoryNode;

struct MemoryNode* createMemoryNode(int size, struct MemoryNode* parent, int start_address) ;
void deallocateMemory(MemoryNode* node);
struct MemoryNode* allocate(int sizep, struct MemoryNode * root);
int get_block_size(int size);
void PreorderTraverse(MemoryNode *root);












#endif 