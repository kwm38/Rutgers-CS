#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
/*
 * This struct will be the building blocks for our binary tree.
 */
struct Node
{
    char * word;
    struct Node *left;
    struct Node *right;
    
}typedef Node;
/*
 * insert is a function that will traverse the tree, comparing each nodes string with the new string ready for insert.
 * The new string will be placed in its sorted order, where a new node will be made to hold the string.
 */
Node* insert(Node *head, char* temp, int size);
/*
 * printAndFree traverses the string, printing all words starting with capital letters first, then lowercase.
 * ex: (A->Z->a->z)
 * Once a node has had all of the strings less than its string printed, it will print its node, then print the strings greater then it.
 * Once a node is no longer needed, its memory will be freed.
 */
void printAndFree(Node * head);
