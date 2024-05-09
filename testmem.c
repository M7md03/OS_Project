#include "headers.h"

int main(void) {
    struct MemoryNode *root = createMemoryNode(MaxSize, NULL, 0);
    struct MemoryNode *nodeA = allocate(70, root);
    // process.Mem=NodeA;
    PreorderTraverse(root);
    // first allocation correct
    if (nodeA != NULL) {
        printf("allocated Memory %d \n", nodeA->size);
        printf("starting position %d  and end position %d \n", nodeA->block.start_address, nodeA->block.end_address);
    } else {
        printf("error\n");
    }
    // second allocation
    PreorderTraverse(root);
    struct MemoryNode *nodeB = allocate(35, root);
    if (nodeB != NULL) {
        printf("allocated Memory %d \n", nodeB->size);
        printf("starting position %d  and end position %d \n", nodeB->block.start_address, nodeB->block.end_address);
    } else {
        printf("error\n");
    }
    PreorderTraverse(root);
    struct MemoryNode *nodeC = allocate(80, root);
    // 3RD allocation
    if (nodeC != NULL) {
        printf("allocated Memory %d \n", nodeC->size);
        printf("starting position %d  and end position %d \n", nodeC->block.start_address, nodeC->block.end_address);
    } else {
        printf("error\n");
    }
    deallocateMemory(nodeA);
    PreorderTraverse(root);
    struct MemoryNode *nodeD = allocate(60, root);
    if (nodeD != NULL) {
        printf("allocated Memory %d \n", nodeD->size);
        printf("starting position %d  and end position %d \n", nodeD->block.start_address, nodeD->block.end_address);
    } else {
        printf("error\n");
    }
    PreorderTraverse(root);
    struct MemoryNode *nodeE = allocate(500, root);
    if (nodeE != NULL) {
        printf("allocated Memory %d \n", nodeE->size);
        printf("starting position %d  and end position %d \n", nodeE->block.start_address, nodeE->block.end_address);
    } else {
        printf("error\n");
    }
    PreorderTraverse(root);
    struct MemoryNode *nodeF = allocate(100, root);
    if (nodeF != NULL) {
        printf("allocated Memory %d \n", nodeF->size);
        printf("starting position %d  and end position %d \n", nodeF->block.start_address, nodeF->block.end_address);
    } else {
        printf("error\n");
    }
    PreorderTraverse(root);
    struct MemoryNode *nodeG = allocate(100, root);
    if (nodeG != NULL) {
        printf("allocated Memory %d \n", nodeG->size);
        printf("starting position %d  and end position %d \n", nodeG->block.start_address, nodeG->block.end_address);
    } else {
        printf("error couldn't find enough space\n");
    }
    PreorderTraverse(root);
    deallocateMemory(nodeB);
    printf("deallocated B\n");
    deallocateMemory(nodeD);
    printf("deallocated D\n");
    PreorderTraverse(root);
    deallocateMemory(nodeC);
    printf("deallocated C\n");
    deallocateMemory(nodeE);
    printf("deallocated E\n");
    deallocateMemory(nodeF);
    printf("deallocated F\n");
    deallocateMemory(nodeG);
    printf("deallocated G\n");
    PreorderTraverse(root);
    struct MemoryNode *nodek = allocate(100, root);
    if (nodek != NULL) {
        printf("allocated Memory %d \n", nodek->size);
        printf("starting position %d  and end position %d \n", nodek->block.start_address, nodek->block.end_address);
    } else {
        printf("error couldn't find enough space\n");
    }
    return 0;
}