#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "mymalloc.h"

static char myblock[5000]; /*our pseudo memory where malloc will allocate memory to*/
Node* head; /*first node that represent metadata*/
Node* last; /* last node to be added to the meta data */
int openSpace = 5000;/*total avaliable space (bytes)for dynamic memoty*/
int initialized =0;/* 0 = no meta data yet, 1 = we have atleast 1 metadata node*/

/* this function will attempt to malloc memory in our pseudo memory which is of size 5000 and return a pointer to that new chunk of memory*/
void *mymalloc(unsigned int size, char *file, int line){
    
    /*cannot malloc 0 bytes*/
    if(size ==0){
        
        printf("Can not allocate 0 bytes in FILE: '%s' on LINE: '%d'\n", file, line);
        
        return NULL;
    }
    /*not enough memory its an error , return a NULL pointer*/
    if(openSpace < (sizeof(Node) + size)){
        
        printf("Can not allocate %d bytes in FILE: '%s' on LINE: '%d'\n",size, file, line);
        
        return NULL;
    }
    /*allocate metadata for first node*/
    if(!initialized && openSpace >= (size + sizeof(Node))){
        
        head = (Node*)myblock;
        head->size = openSpace - sizeof(Node);
        head->free = '0';
        head->prev= head->next= NULL;
        initialized = 1;
    }
    
    Node* temp = head;
    
    /* we will look for the first free node that has the amount of space we need at the very least, or more */
    while(temp!=NULL){
        
        /* we have found the first free node that fits */
        if( temp->free == '0' && openSpace >= (size + sizeof(Node))){
            
            openSpace -= size;
            
            /* we have found a node with too much space, so we need to split the space and take what we need */
            if(temp->next != NULL && size < temp->size){
                
                int difference = temp->size - size;
                
                temp->size= (short)size; /* node's size will be of the size we requested to malloc */
                temp->free = '1';/* node is no longer freee */
                Node * newNode = (Node*)((char*)last + sizeof(Node));
                newNode->prev = temp;
                newNode->size = difference;
                newNode->next = temp->next;
                temp->next = newNode;
                
                printf("allocated %d bytes , %d remaining \n", size, openSpace);
                return temp;
                
            }else if(temp->size == size){
                /* we have the exact amount of space we need with a node ready to hold it, so just make the node occupied instead of free */
                temp->free = '1';/* node is no longer freee */
                
                printf("allocated %d bytes , %d remaining \n", size, openSpace);
                return temp;
                
            }else{
                /* the only avaliable space is at the end portion of avaliable memory */
                
                temp->size= (short)size; /* node's size will be of the size we requested to malloc */
                temp->free = '1';/* node is no longer free */
                
                if((int)(openSpace - sizeof(Node)) >= 0){
                    
                    openSpace -= sizeof(Node);
                    Node * newNode = (Node*)((char*)temp + sizeof(Node));
                    newNode->prev = temp;
                    newNode->free = '0';
                    newNode->size = openSpace;
                    newNode->next = NULL;
                    temp->next = newNode;
                    last = newNode;
                    
                }else{
                    
                    temp->next = NULL;
                    last=temp;
                }
                
                printf("allocated %d bytes , %d remaining \n", size, openSpace);
                return temp;
            }
        }
        
        temp = temp->next;
    }
    /* we were unable to find space for the size you wished to malloc */
    printf("Can not allocate %d bytes in FILE: '%s' on LINE: '%d'\n",size, file, line);
    return NULL;
    
}

/* this function will take the pointer you wish to free and attempt to free it from our pseudo dynamic memory */
void myfree(void *p, char *file, int line){
    
    
    Node * freeMe = (Node*)((char*)p);
    
    if(freeMe ->free == 0){
        
        printf("Error: Memory was not allocated for the pointer in %s on line: %d, so it can not be freed\n", file, line);
        return;
        
    }
    
    /* If you wished to free the head when it is just 1 single metadata node  */
    if(freeMe == head && head->size == (5000-sizeof(Node))){
        
        openSpace += sizeof(Node);
        head = NULL;
        initialized =0;
        
        printf("freed %d bytes successfully , %d avaliable\n", (int)sizeof(Node), openSpace);
        return;
    }
    /* If you wished to free an already free node that would be an error  */
    if(freeMe ->free == '0'){
        
        printf("Error: Memory has already been freed in %s on line :%d\n", file, line);
        return;
    }
    int size = (int)freeMe->size;
    
    freeMe->free = '0';
    openSpace += (freeMe->size);
    
    Node* prev = (Node*)freeMe->prev;
    Node* next =(Node*) freeMe->next;
    
    /* We will check to see if we can merge with the free node previous to this pointer  */
    if(prev!= NULL && prev->free == '0'){
        
        prev->size += (short)(size + sizeof(Node));
        prev->next = freeMe->next;
        
        if(freeMe->next != NULL){
            freeMe->next->prev = prev;
        }else{
            prev->next = NULL;
        }
        openSpace += sizeof(Node);
        /* We will check to see if we can merge with the free node after this pointer  */
        if(next!= NULL && next->free == '0'){
            
            prev->size += (short)(next->size + sizeof(Node));
            prev->next = next->next;
            if(next->next !=NULL){
                next->next->prev = freeMe;
            }else{
                prev->next = NULL;
            }
            openSpace += sizeof(Node);
            next = NULL;
        }
        freeMe = NULL;
    }
    /* prev node was not free but We will check to see if we can merge with the free node after this pointer  */
    if(freeMe !=NULL && next!= NULL && next->free == '0'){
        
        next->size +=(short)(size + sizeof(Node));
        
        
        if(freeMe->prev !=NULL){
            next->prev = freeMe->prev;
            prev-> next = next;
        }else{
            next->prev = NULL;
        }
        openSpace += sizeof(Node);
        freeMe=NULL;
        
    }
    
    printf("freed %d bytes successfully , %d avaliable\n", size, openSpace);
    return;
    
}



