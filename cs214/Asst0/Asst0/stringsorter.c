#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "stringsorter.h"
/*
 * insert is a function that will traverse the tree, comparing each nodes string with the new string ready for insert.
 * The new string will be placed in its sorted order, where a new node will be made to hold the string.
 */
Node* insert(Node *head, char* temp, int size){
    
    if(head == NULL){
        head = (Node*)malloc(sizeof(Node));
        head->word = (char*)malloc(sizeof(char)*(size +1));
        strcpy(head->word,temp);
        head->left = head->right = NULL;
        return head;
    }
    if(strcmp(head->word, temp) <=0 ){
        head->left = insert(head->left, temp, size);
    }else{
        head->right = insert(head->right, temp, size);
    }
    return head;
}
/*
 * printAndFree traverses the string, printing all words starting with capital letters first, then lowercase.
 * ex: (A->Z->a->z).
 * Once a node has had all of the strings less than its string printed, it will print its node, then print the strings greater then it.
 * Once a node is no longer needed, its memory will be freed.
 */
void printAndFree(Node * head){
    
    if(head->right != NULL){
        printAndFree(head->right);
    }
    printf("%s\n", head->word);
    if(head->left != NULL){
        printAndFree(head->left);
    }
    free(head);
}
int main(int argc, char** argv){
    
    /*
     * A check is made to insure the user has infact passed only 1 arguement, no more, no less.
     * If there is more or less then one arguement, it is an error.
     * The user is informed and the program exits.
     */
    if(argc != 2){
        printf("An invalid number of  arguements, please only have one arguement\n");
        exit(0);
    }
    /*
     * Here all of the necessary pointers are created to parse the argument for strings into a binary tree.
     */
    char* original = argv[1];
    char* pointer = original;
    int sizeOfWord = 0;
    int posInStr = 0;
    Node* head = NULL;
    /*
     * The argument is parsed and the binary tree is built so long as the argument is not empty, or all seperators.
     */
    while(pointer[0] != '\0'){
        if(isalpha(pointer[0]) == 0){
            pointer ++;
            posInStr++;
            continue;
        }
        while(isalpha(pointer[0]) != 0){
            sizeOfWord++;
            pointer++;
        }
        char * temp = (char*)malloc(sizeof(char) * sizeOfWord + 1);
        strncpy(temp, original + posInStr, sizeOfWord);
        temp[sizeOfWord] = '\0';
        head = insert(head,temp,sizeOfWord);
        posInStr += sizeOfWord;
        sizeOfWord = 0;
    }
    /*
     * If the tree is not null, we will print its strings in an A->Z->a->z order, and free the nodes once we are done with them.
     */
    if(head != NULL){
        printAndFree(head);
    }
    return 0;
}


