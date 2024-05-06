#include "memory.h"
#include "memory.c"
int main(void)
{
    struct MemoryNode *root = createMemoryNode(MaxSize, NULL, 0);
    struct MemoryNode *nodeA = allocate(70, root);
    //first allocation correct 
    if(nodeA != NULL)
    {
        printf("allocated Memory %d \n", nodeA->size);
        printf("starting position %d  and end position %d \n", nodeA ->block.start_address, nodeA->block.end_address);
    }
    else
    {
        printf("error\n");
    }
    //second allocation 
    struct MemoryNode *nodeB = allocate(35, root);
    if(nodeB != NULL)
    {
        printf("allocated Memory %d \n", nodeB->size);
        printf("starting position %d  and end position %d \n", nodeB ->block.start_address, nodeB->block.end_address);
    }
    else
    {
        printf("error\n");
    }
    struct MemoryNode *nodeC = allocate(80, root);
    //3RD allocation 
    if(nodeC != NULL)
    {
        printf("allocated Memory %d \n", nodeC->size);
        printf("starting position %d  and end position %d \n", nodeC ->block.start_address, nodeC->block.end_address);
    }
    else
    {
        printf("error\n");
    }
    deallocateMemory(nodeA);
    struct MemoryNode *nodeD = allocate(60, root);
    if(nodeD != NULL)
    {
        printf("allocated Memory %d \n", nodeD->size);
        printf("starting position %d  and end position %d \n", nodeD->block.start_address, nodeD->block.end_address);
    }
    else
    {
        printf("error\n");
    }
    deallocateMemory(nodeB);
    deallocateMemory(nodeD);
    deallocateMemory(nodeC);
    return 0;
}