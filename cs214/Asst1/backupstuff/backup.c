#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "mymalloc.h"
/* this function will attempt to malloc memory in our pseudo memory which is of size 5000 and return a pointer to that new index of allocated memory*/
void *mymalloc(unsigned int size, char *file, int line){
    
    /* pointer to the first two bytes in memory that keep track of the avaliable space left in the array */
    short *  openSpace = (short*)(myblock);
    
    /* check to see if myblock was initialized, if not initialize it with 4998 bytes and myblock[3] = END OF TEXT (ETX) -- signifies free metadata node*/
    if(*openSpace == 0 && size <= 4997){
        openSpace = (short*)(myblock);
        *openSpace =(short) (4997 - size);
        
        /* ACKNOWLEDGE (ACK)  = decimal value '~' , which means memory is taken*/
        myblock[2]  = '~';
      //  short *  takenSpace = (short*)(myblock+3);
      //  *takenSpace = (short)size;
        
        if(*openSpace>=3){
            /* Negative Acknowledge (NAK)  = decimal value '|' , which means memory is free*/
           myblock[2 + size +1] = '|';//  myblock[5 + size] = '|';
           // short *  freeSpace = (short*)(myblock+ 5 + size + 1);
            *openSpace -= 1;//3;
            
            //*freeSpace = *openSpace;
        }
        void * returnAddress = ((void*)(myblock + 3));
        //printf("Successfully allocated %d bytes in FILE: '%s' on LINE: '%d'\n",size, file, line);
        return returnAddress;
    }
    
    /* if there is not enough open space or you can not allocated 4997 or more bits. Can not allocated 0 or less bits */
    
    if((*openSpace - size) <= 0 || size > *openSpace  || size <=0){
        
        printf("Can not allocate %d bytes in FILE: '%s' on LINE: '%d'\n",size, file, line);
        return 0;
    }
    int i =0; /* used for incrementing through the memory */
    int index =0;/* used to equate the size of free space avaliable */
    int potential =0;/* used to mark a potential free space for our pointer */
    
    for(i=2; i<5000; i++){
        
        /* if this block could be used, it is potentially where we will allocate the pointer
         * remember 19 means the space is free and potential = 0 means we havent found a free space that fits yet
         */
        if(potential == 0 && myblock[i] == '|'){
            index = i;
            potential = 1;
            continue;
        }
        /* we have a potential free space, but we need to make sure the space fits our size
         * potential = 1 means we have a free space, 20 means we have found an index that is taken, so lets subtract the difference
         * i= 4999 means we hit the end of the memory
         */
        if(potential == 1 && (myblock[i] == '~' || i == 4999)){
            
            /* get the size of the free space, if it fits lets update the metadata to taken('~') and update its size and the openspace size*/
          if( (i - index -1 ) >= size){ //    if( (i - index - 3) >= size){
                
                myblock[index] = '~';
                //short *  takenSpace = (short*)(myblock + index +1);
               // *takenSpace = (short) size;
                *openSpace -= (short) size;
              
                if( (i - index -1 ) == size){  //if( (i - index - 3) == size){
                    void * returnAddress = ((void*)(myblock + index + 1));//((void*)(myblock + index + 3));
                    return returnAddress;
                }
                /* make new metadata for remaining free sapce if we have the space, and there is no metadata following our new metadata*/
                if(i == 4999 && (index + size ) <= 4996){// if(i == 4999 && (index + 2 + size +4 ) <= 4999){
                   
                    myblock[index + size +1] = '|';// myblock[index +2 + size + 1] = '|';
                    //short *  freeSpace = (short*)(myblock + index +2 + size + 2);
                    *openSpace -= (short) 1;//*openSpace -= (short) 3;
                   // *freeSpace = *openSpace;
                    
                    /* we didn't hit the end of memory, so we hit a free node, lets try and split if we have excess memory*/
                 }else if((i - index - 1 - size ) >=2){// }else if(((int)(i - index - 2 - size - 4)) >0){
                    
                 myblock[index +1 + size] = '|';  //  myblock[index +2 + size + 1] = '|';
                   // short *  newFreeSpace = (short*)(myblock + index +2 + size + 2);
                    *openSpace -= 1; // cahgned to 1 from 3
                   // *newFreeSpace = (short)(i - index - 2 - size - 4);
                    
                    
                    /* we couldnt split so account for extra6 given space */
                 }else if((i - index -1 - size) < 2){ // }else if((i - index -2 - size) < 4){
                    
                    //*takenSpace += (short)(i - index -2 - size);
                    *openSpace -= (short)(i - index -1 - size);//*openSpace -= (short)(i - index -2 - size);
                }
                
                //printf("Successfully allocated %d bytes in FILE: '%s' on LINE: '%d'\n",size, file, line);
               void * returnAddress = ((void*)(myblock + index + 1)); //void * returnAddress = ((void*)(myblock + index + 3));
                return returnAddress;
                
            }else{
                potential = 0;
                //short *  takenSpace = (short*)(myblock + i +1);
                //i += ( 3 + *takenSpace);
                i++;
            }
        }
    }
    /* out of all the free spaces, there were none that fit the size you were trying to allocate memory for*/
    printf("Can not allocate %d bytes in FILE: '%s' on LINE: '%d'\n",size, file, line);
    return 0;
}

void myfree(void *p, char *file, int line){
    
    /*checks to make sure you are freeing a pointer that was in fact malloced by our simulaotr */
    if(p == NULL || ((char*)p)[-1] == 0 || (char*)p > (myblock + 4999) || (char*)p < myblock || ((char*)p)[-1] != '~'){
   
        printf("Error: Memory was not allocated for the pointer in %s on line: %d, so it can not be freed \n",file, line);
        return;
    }
    short *  openSpace = (short*)(myblock);
    
     int   i = 0;//int   i = 1;//*((short*)(p - 2));
    int j=-2; //int j=-4;
    char * left = (char *) p;
    char * right = (char *) p;
    int leftDone =0;
    int rightDone=0;
    
     ((char*)p)[-1] = '|';// ((char*)p)[-3] = '|';
    
    //short * mySize = (short*)(p - 2);
    
    /* this while loop will attempt to merge our newley freed space with any free space to the left or right of our new free space */
    
    
    while (!leftDone || !rightDone ){
        
        if(rightDone ==0 && (char*)(right+i) == (char*)(myblock + 5000)){
            
            *openSpace += i;//*mySize;
            rightDone = 1;
        }
        if(leftDone ==0 && ((char)left[j] == '~' || (char*)(left+j) == (char*)(myblock+1)) ){
            
            leftDone = 1;
        }
        if(rightDone ==0 && (char)right[i] == '~'){
            
            *openSpace += i;//*mySize;
            rightDone = 1;
        }
        if(rightDone ==0 && ((char*)p)[i] == '|'){
            //short * freeSize = (short*)(right+i+1);
            *openSpace += (short)(i + 1) ;//(*mySize + 3);
            //  *mySize += (*freeSize +3);
            
            printf("here  %d -- %c\n",i, ((char*)p)[i]);
            right[i] =0;
            rightDone =1;
            
        }
        if(leftDone ==0 && left[j] == '|'){
            if(rightDone){
              //  short * freeSize = (short*)(left+j+1);
               // *freeSize += (*mySize +3);
                *openSpace += 1 ;//( 3);
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
    /* this means the only pointer left in the array is 1 pointer representing all the free spacec, so just remove it */
    printf("here dingg %d\n",*openSpace);
    if(*openSpace == 4997){
        
        myblock[1] = 0;
        myblock[2] = 0;
        myblock[2] = 0;
        myblock[3] = 0;
        myblock[4] = 0;
        *openSpace = 0;
    }
    
    //printf("Successfully freed a pointer in FILE: '%s' on LINE: '%d'\n", file, line);
    return;
}



