#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "mymalloc.h"

/*our pseudo memory where malloc will allocate memory to*/
static char myblock[5000];

/* this function will attempt to malloc memory in our pseudo memory which is of size 5000 and return a pointer to that new index of allocated memory
 * our meta data is one character long, and we use a short to keep track of the free sapce, so the actualy total ammount of malloc()able space is 4997 bytes.
 */
void *mymalloc(unsigned int size, char *file, int line){
    
    /* pointer to the first two bytes in memory that keep track of the avaliable space left in the array */
    short *  openSpace = (short*)(myblock);
    
    /* check to see if myblock was initialized, if not initialize it with 4997 bytes and myblock[3] = '~' -- which signifies a taken metadata node*/
    if(*openSpace == 0 && size <= 4997){
        
        openSpace = (short*)(myblock);
        *openSpace =(short) (4997 - size);
        myblock[2]  = '~';
        
        if(*openSpace>=3){
            /* '|' is used to represent metadata for free blocks of memory*/
            myblock[2 + size +1] = '|';
            *openSpace -= 1;
        }
        
        /*printf("Successfully allocated %d bytes in FILE: '%s' on LINE: '%d'\n",size, file, line);*/
        void * returnAddress = ((void*)(myblock + 3));
        return returnAddress;
    }
    
    /* if there is not enough open space or you can not allocated 4997 or more bits or you tried to allocated 0 or less bits, this is an error */
    if((*openSpace - size) <= 0 || size > *openSpace  || size <=0){
        
        printf("Can not allocate %d bytes in FILE: '%s' on LINE: '%d'\n",size, file, line);
        return 0;
    }
    int i =0; /* used for incrementing through the memory */
    int index =0;/* used to equate the size of free space avaliable */
    int potential =0;/* used to mark a potential free space for our pointer */
    
    for(i=2; i<5000; i++){
        
        /* if this block could be used, it is potentially where we will allocate the pointer
         * remember '|' means the space is free and potential = 0 means we haven't found a free space that fits yet
         */
        if(potential == 0 && myblock[i] == '|'){
            index = i;
            potential = 1;
            continue;
        }
        /* we have a potential free space, but we need to make sure the space fits our size
         * potential = 1 means we have a free space, "~" means we have found an index that is taken, so lets subtract the difference
         * i= 4999 means we hit the end of the memory
         */
        if(potential == 1 && (myblock[i] == '~' || i == 4999)){
            
            /* get the size of the free space, if it fits lets update the metadata to taken('~') and update its size and the openspace size*/
            if( (i - index -1 ) >= size){
                
                myblock[index] = '~';
                *openSpace -= (short) size;
                /* if we have exactly the ammount of space we need, then we can just return the pointer, no need to do anything else */
                if( (i - index -1 ) == size){
                    
                    void * returnAddress = ((void*)(myblock + index + 1));
                    return returnAddress;
                }
                /* The '|' we found is metadata for all of the free space remaining to the right of the last '~'.
                 * So lets create another '|' to mark the new remaining free space
                 */
                if(i == 4999 && (index + size ) <= 4996){
                    
                    myblock[index + size +1] = '|';
                    *openSpace -= 1;
                    
                    /* we didn't hit the end of memory, so we hit a free node ('|'), lets try and split this memory if we have excess memory*/
                }else if((i - index - 1 - size ) >=2){
                    
                    myblock[index +1 + size] = '|';
                    *openSpace -= 1;
                    
                    /* we couldn't split so we will malloc() a couple extra bytes, so we need to account for extra given space */
                }else if((i - index -1 - size) < 2){
                    
                    *openSpace -= (short)(i - index -1 - size);
                }
                
                /*printf("Successfully allocated %d bytes in FILE: '%s' on LINE: '%d'\n",size, file, line);*/
                
                void * returnAddress = ((void*)(myblock + index + 1));
                return returnAddress;
                
                /* Our potential space was not a fit for our size, so look for the next free chunck represented by '|' */
            }else{
                potential = 0;
                i++;
            }
        }
    }
    /* Out of all the free spaces, there were none that fit the size you were trying to allocate memory for, so we return a NULL pointer*/
    printf("Can not allocate %d bytes in FILE: '%s' on LINE: '%d'\n",size, file, line);
    return 0;
}
/* This function will only free memory that was alloced by our mymalloc function */
void myfree(void *p, char *file, int line){
    
    /*checks to make sure you are freeing a pointer that was in fact malloced by our simulaotr */
    if(p == NULL || (char*)p > (myblock + 4999) || (char*)p < myblock || ((char*)p)[-1] == 0 || ((char*)p)[-1] != '~'){
        
        printf("Error: Memory was not allocated for the pointer in %s on line: %d, so it can not be freed \n",file, line);
        return;
    }
    short *  openSpace = (short*)(myblock);/* the open space remaining in the memory */
    int i = 0; /* counter used to traverse to the right in memory */
    int j=-2; /* counter used to traverse to the left in memory */
    char * left = (char *) p;/* pointer used to traverse to the right in memory */
    char * right = (char *) p;/* pointer used to traverse to the left in memory */
    int leftDone =0;/* will let us know if we are done traversing right */
    int rightDone=0;/* will let us know if we are done traversing left */
    
    ((char*)p)[-1] = '|';/* change our metadata to be '|', meaning the metadata represents free memory*/
    
    /* this while loop will attempt to merge our newley freed space with any free space to the left or right of our new free space */
    while (!leftDone || !rightDone ){
        
        /* if we have gone to the right most bound, or found a taken chunk of memory, so we can not merge */
        if(rightDone ==0 &&  (char*)(right+i) == (char*)(myblock + 5000)){
            
            *openSpace += i;
            rightDone = 1;
        }
        if(rightDone ==0 && (char)right[i] == '~'){
            
            *openSpace += i;
            rightDone = 1;
        }
        /* if we have gone to the left most bound, or found a taken chunk of memory, so we can not merge */
        if(leftDone ==0 && ((char)left[j] == '~' || (char*)(left+j) == (char*)(myblock+1)) ){
            
            leftDone = 1;
        }
        /* we have found a chunk of memory to the right we can merge with, so remove the new found metadata */
        if(rightDone ==0 && ((char*)right)[i] == '|'){
            
            *openSpace += (short)(i + 1) ;
            right[i] =0;
            rightDone =1;
        }
        /* we have found a chunk of memory to the left we can merge with, so remove our metadata  */
        if(leftDone ==0 && left[j] == '|'){
            /* we must first make sure whether or not we have merged on the right first before merging left
             * if not keep track of this index by keeping j where it is
             */
            if(rightDone){
                *openSpace += 1 ;
                ((char*)p)[-1] = 0;
                leftDone =1;
                p = NULL;
                
            }else{
                
                j++;
            }
        }
        
        i++;
        j--;
        
    }
    /* this means the only pointer left in the array is 1 pointer representing all the free space, so just remove it, and restore the memory to back to being completely empty*/
    if(*openSpace == 4997){
        myblock[1] = 0;
        myblock[2] = 0;
        myblock[3] = 0;
        myblock[4] = 0;
        *openSpace = 0;
    }
    
    //printf("Successfully freed a pointer in FILE: '%s' on LINE: '%d'\n", file, line);
    return;
}



