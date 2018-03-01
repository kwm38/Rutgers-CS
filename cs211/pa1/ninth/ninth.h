#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>



struct Node
{	
    int key;
    int less_eq;
    struct Node *left;
    struct Node *right;

}typedef Node;




Node *  deleteMe(Node * root, int val);
void insert( Node* head, int val);

void search (Node* head, int val);

void clearSpace(Node *head);
