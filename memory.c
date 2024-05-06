#include "memory.h"

// Function to create a new memory node

struct MemoryNode* createMemoryNode(int size, struct MemoryNode* parent, int start_address)     
{
    struct MemoryNode* node = (struct MemoryNode*)malloc(sizeof(struct MemoryNode));
    node->size = size;
    node->block.start_address = start_address;
    node->block.end_address = start_address + size - 1;
    node->left_child = NULL;
    node->right_child = NULL;
    node->parent = parent;
    node ->used = 0;
    return node;
}
//takes a process and alocates it in memory if there is space else it return false (no space)
struct MemoryNode* allocate(int Psize, struct MemoryNode * root)
{
    if(root == NULL) return NULL;
    if(root->used == 0)
    {
        int num = root->size;int half = num/2;
        if(Psize < half)
        {
            if(root->left_child == NULL && root->right_child == NULL)
            {
                //create children
                root->left_child = createMemoryNode(half, root, root->block.start_address);
                root->right_child = createMemoryNode(half, root, (root->left_child->block.end_address)+1);
                printf("creating left and right children\n");
                if( half/2 < Psize)
                {
                    root->left_child->used = 1;
                    printf("Allocating in created left child\n");
                    return root->left_child;
                }
                else
                {
                    MemoryNode* left_result = allocate(Psize,root->left_child);
                    if(left_result != NULL)
                    {
                        left_result->used = 1;
                        return left_result;
                    }
                }
            }
            else
            {
                MemoryNode* left_result = allocate(Psize,root->left_child);
                if(left_result != NULL)
                {
                    left_result->used = 1;
                    printf("Allocated in existing LEFT child \n");
                    return left_result;
                }
                else
                {
                    MemoryNode* right_result = allocate(Psize,root->right_child);
                    if (right_result != NULL)
                    {
                        printf("Allocated in existing RIGHT child \n");
                        right_result->used = 1;
                        return right_result;
                    }
                }
            }
        }
        else
        {
            if(root->left_child == NULL && root->right_child == NULL)
            {
                root->used = 1;
                return root;
            }
            else
            {
                return NULL;
            }
        }
    }
    else
    {
        return NULL;
    }
}
// void deallocateMemory(struct MemoryNode* node)       // Function to deallocate memory
//  {
//     if (node == NULL) {
//         return;
//     }

//     if (node->left_child == NULL && node->right_child == NULL) {
//         // deallocate memory and recursively update parent nodes
//         struct MemoryNode* parent = node->parent;
//         if (parent != NULL)                                     //root of the tree no where to go
//         {
//             if (parent->left_child == node) 
//             {
//                 parent->left_child->used = 0;
//                 printf("FREEING MEMORY FROM %d to %d \n", parent->left_child->block.start_address,parent->left_child->block.end_address);
//                 free(parent->left_child);
//                 parent->left_child = NULL;
//                 if(parent->right_child->used == 0)              //right child is free
//                 {
//                     free(parent->right_child);
//                     parent->right_child = NULL;
//                     deallocateMemory(parent);                    //go deallocate the parent to merge back memory if needed
//                 }
//             } 
//             else
//             {
//                 parent->right_child->used = 0;                  //the node is the right child
//                 printf("FREEING MEMORY FROM %d to %d \n", parent->left_child->block.start_address,parent->left_child->block.end_address);
//                 free(parent->right_child);
//                 parent->right_child = NULL;
//                 if(parent->left_child->used == 0)              //left child is free
//                 {
//                     free(parent->left_child);
//                     parent->left_child = NULL;
//                     deallocateMemory(parent);                   //go deallocate the parent to merge back memory if needed
//                 }
//             }
//         }
//     }
// }
void deallocateMemory(struct MemoryNode* node)       // Function to deallocate memory
{
    if (node == NULL) {
        return;
    }

    if (node->left_child == NULL && node->right_child == NULL) {
        // deallocate memory and recursively update parent nodes
        struct MemoryNode* parent = node->parent;
        if (parent != NULL)                                     //root of the tree no where to go
        {
            if (parent->left_child == node) 
            {
                parent->left_child->used = 0;
                printf("FREEING MEMORY FROM %d to %d \n", parent->left_child->block.start_address,parent->left_child->block.end_address);
                free(parent->left_child);
                parent->left_child = NULL;
                if(parent->right_child != NULL && parent->right_child->used == 0)              //right child is free
                {
                    printf("FREEING MEMORY FROM %d to %d \n", parent->right_child->block.start_address,parent->right_child->block.end_address);
                    free(parent->right_child);
                    parent->right_child = NULL;
                    deallocateMemory(parent);                    //go deallocate the parent to merge back memory if needed
                }
            } 
            else
            {
                parent->right_child->used = 0;                  //the node is the right child
                printf("FREEING MEMORY FROM %d to %d \n", parent->right_child->block.start_address,parent->right_child->block.end_address);
                free(parent->right_child);
                parent->right_child = NULL;
                if(parent->left_child != NULL && parent->left_child->used == 0)              //left child is free
                {
                    printf("FREEING MEMORY FROM %d to %d \n", parent->left_child->block.start_address,parent->left_child->block.end_address);
                    free(parent->left_child);
                    parent->left_child = NULL;
                    deallocateMemory(parent);                   //go deallocate the parent to merge back memory if needed
                }
            }
        }
    }
}

