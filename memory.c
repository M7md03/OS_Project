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
MemoryNode* findBestFitNodeOfSize(MemoryNode* root, int size) {
    
    if (root == NULL)
    {
        return NULL; // If tree is empty, return NULL
    }

    MemoryNode* bestFit = NULL; // Initialize the best fit node to NULL

    // Traverse the tree in preorder
    if (root->left_child != NULL) 
    {
        bestFit = findBestFitNodeOfSize(root->left_child, size);
    }

    // Check if the current node is a leaf node
    if (root->left_child == NULL && root->right_child == NULL) 
    {
        // If the leaf node has the required size and is free
        if (root->size >= size && root->used == 0 && (root->size)/2 < size) 
        {
            // If bestFit is NULL or the current node's size is smaller than the bestFit's size
            if (bestFit == NULL || root->size < bestFit->size) 
            {
                bestFit = root; // Update bestFit to the current node
            }
        }
    }

    if (root->right_child != NULL) 
    {
        MemoryNode* rightBestFit = findBestFitNodeOfSize(root->right_child, size);
        if (rightBestFit != NULL && (bestFit == NULL || rightBestFit->size < bestFit->size)) 
        {
            bestFit = rightBestFit;
        }
    }
    return bestFit; // Return the best fit node found
}

//takes a process and alocates it in memory if there is space else it return false (no space)
struct MemoryNode* allocate(int Psize, struct MemoryNode * root)
{
    if(root == NULL) return NULL;
    MemoryNode * Bestfit = findBestFitNodeOfSize(root,Psize);
    if(Bestfit != NULL)
    {
        Bestfit->used = 1;
    }
    else
    {
        if(root->used == 0)
        {
            int num = root->size;int half = num/2;
            if(Psize < half)                       //don't place in root
            {
                if(root->left_child == NULL && root->right_child == NULL)           //if root has no children
                {
                    //create children
                    root->left_child = createMemoryNode(half, root, root->block.start_address);
                    root->right_child = createMemoryNode(half, root, (root->left_child->block.end_address)+1);
                    printf("creating left and right children\n");
                    if( half/2 == Psize)
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
}

void deallocateMemory(struct MemoryNode* node)      // Function to deallocate memory
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
                if(parent->right_child != NULL )              //right child is free
                {
                    if(parent->right_child->left_child ==NULL && parent->right_child->right_child ==NULL && parent->right_child->used == 0)
                    {
                        printf("FREEING MEMORY FROM %d to %d \n", parent->right_child->block.start_address,parent->right_child->block.end_address);
                        free(parent->right_child);
                        free(parent->left_child);
                        parent->right_child = NULL;
                        parent->left_child = NULL;
                        deallocateMemory(parent);                    //go deallocate the parent to merge back memory if needed
                    }
                }
                return;
            } 
            else if(parent->right_child == node)
            {
                parent->right_child->used = 0;                  //the node is the right child
                printf("FREEING MEMORY FROM %d to %d \n", parent->right_child->block.start_address,parent->right_child->block.end_address);
                if(parent->left_child != NULL)              //left child is free
                {
                    if(parent->left_child->left_child ==NULL && parent->left_child->right_child ==NULL && parent->left_child->used == 0)
                    {
                        printf("FREEING MEMORY FROM %d to %d \n", parent->left_child->block.start_address,parent->left_child->block.end_address);
                        free(parent->left_child);
                        free(parent->right_child);
                        parent->left_child = NULL;
                        parent->right_child = NULL;
                        deallocateMemory(parent);                   //go deallocate the parent to merge back memory if needed
                    }
                }
                return;
            }
        }
        else
        {
            //the node is the root of the tree
            return;
        }
    }
    //if node has children
    if(node->left_child->used == 0 && node->right_child->used == 0)
    {
        free(node->left_child);
        free(node->right_child);
        node->left_child = NULL;
        node->right_child = NULL;
        deallocateMemory(node->parent);
    }
    return;
}

//preorder traversal of the tree prints the leaves
void PreorderTraverse(MemoryNode *root)
{
    if (root == NULL)
        return;
    PreorderTraverse(root->left_child);
    PreorderTraverse(root->right_child);

    if (root->left_child == NULL && root->right_child == NULL)
        printf("block_size:%d start:%d end:%d used: %d \n ", root->size, root->block.start_address, root->block.end_address,root->used);
}