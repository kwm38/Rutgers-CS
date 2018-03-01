#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include "mymalloc.h"

int main(int argc, char ** argv){
    
    printf("------------------------------------------------------------------------------------------------------------------\n");
    
    /* these variables will keep track of the average time for running each test case 100 times */
    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0;
    double e = 0;
    double f = 0;
    
    /* Insert this loop anywhere in the test cases to have visual representation of our pseudo memory
     * - equates to memory
     * T equates to a node representing taken memory ( where the memory it points to is to the right of it , ending 1 index before the next t, or f)
     * F equates to free memoery ( where the memory it points to is to the right of it , ending 1 index before the next t, or f)
     
     int x= 0;
     for(x=0; x< 5000; x++){
     
     if(startOfMem[x-3] == '|'){
     
     printf("F");
     }else   if(startOfMem[x-3] == '~'){
     
     printf("T");
     }else{
     printf("-");
     
     }
     
     }
     
     */
    
    
    /* seed for our rand() calls */
    srand(time(NULL));
    
    /* DETECTABLE ERRORS */
    
    /* Free()ing pointers that were not allocated by malloc(): */
    printf("\nerror check one\n");
    
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    
    
    char * p = (char *)malloc( 200 );
    
    /* char *startOfMem =p;
     * use this with the loop above to display the memory
     */
    free( p + 10 );
    
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
    printf("error check one time: %.15lf\n", elapsed);
    
    /* Redundant free()ing of the same pointer: */
    printf("\nerror check two\n");
    
    gettimeofday(&begin, NULL);
    
    free(p);
    free(p);
    
    gettimeofday(&end, NULL);
    elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
    printf("error check two time: %.15lf\n", elapsed);
    
    /* Saturation of dynamic memory:  -- metadata size is one byte and our short to keep track of space is two bytes.
     * Max space is 4997 per memory allocation
     */
    printf("\nerror check three\n");
    
    gettimeofday(&begin, NULL);
    
    p = malloc (5000);
    
    p = malloc (4999);
    
    
    gettimeofday(&end, NULL);
    elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
    printf("error check three time: %.15lf\n", elapsed);
    
    /* we will perform each test case 100 times and get the average time it takes for each test case*/
    int count =0;
    while(count !=100){
        
        srand(time(NULL));
        
        /* A. malloc() 1 byte and immediately free it - do this 150 times */
        
        gettimeofday(&begin, NULL);
        
        int i =0;
        
        for(i = 0; i < 150; i++){
            
            p = malloc(1);
            free(p);
        }
        
        gettimeofday(&end, NULL);
        elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
        a+= elapsed;
        
        
        /* B. malloc() 1 byte, store the pointer in an array - do this 150 times.
         * Once you've malloc()ed 150 byte chunks, then free() the 150 1 byte pointers one by one.
         */
        gettimeofday(&begin, NULL);
        
        void * array [150];
        
        for(i = 0; i < 150; i++){
            
            array[i] = malloc(1);
        }
        for(i = 0; i < 150; i++){
            
            free(array[i]);
        }
        
        gettimeofday(&end, NULL);
        elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
        b+= elapsed;
        
        
        /*  C. Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer - do this 150 times */
        int garbage = 8;
        for(i=0;i<150;i++){
            
            array[i] = &garbage;
            
        }
        
        gettimeofday(&begin, NULL);
        
        int malloced = 0;
        int occupied = 0;
        int done =0;
        
        while(!done){
            
            int r = rand()% 20;
            
            if(malloced != 150 && r > 15){
                /* if we havent malloced 150 times yet, malloc 1 byte */
                int index = rand() % 150;
                
                if(array[index] == &garbage){
                    array[index] = malloc(1);
                    malloced ++;
                    occupied++;
                }
                
            }else{
                /* pick a random index between 0-150 and try to free it
                 */
                int index = rand() % (150);
                
                if(array[index] != &garbage){
                    free(array[index]);
                    array[index] = &garbage;
                    occupied--;
                }
            }
            if(malloced == 150 && occupied == 0){
                
                done =1 ;
                
            }
        }
        
        gettimeofday(&end, NULL);
        elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
        c+= elapsed;
        
        /* D. Randomly choose between a randomly-sized malloc() or free()ing a pointer – do this many times (see below)
         
         - Keep track of each malloc so that all mallocs do not exceed your total memory capacity
         
         - Keep track of each operation so that you eventually malloc() 150 times
         
         - Keep track of each operation so that you eventually free() all pointers
         
         - Choose a random allocation size between 1 and 64 bytes
         */
        gettimeofday(&begin, NULL);
        
        malloced = 0;
        occupied = 0;
        done =0;
        
        while(!done){
            
            int r = rand()% 20;
            
            if(malloced != 150 && r > 15){
                /* if we havent malloced 150 times yet, malloc a random number of bytes */
                int index = rand() % 150;
                
                if(array[index] == &garbage){
                    
                    int size = (rand() % 64) + 1;
                    array[index] = malloc(size);
                    malloced ++;
                    occupied++;
                }
                
            }else{
                /* pick a random index between 0-500 and try to free it
                 */
                int index = rand() % (150);
                
                if(array[index] != &garbage){
                    free(array[index]);
                    array[index] = &garbage;
                    occupied--;
                }
            }
            if(malloced == 150 && occupied == 0){
                
                done =1 ;
                
            }
        }
        
        gettimeofday(&end, NULL);
        elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
        d+= elapsed;
        
        
        
        /* E,F: Two more workloads of your choosing */
        
        /* E. Pick a random number of bytes 2 - 64 and malloc until you can no longer malloc
         * once we have mallocec as many as we can, free every other pointer: 0, 2 ,4 ....
         * now pick another random number that fits in the free space without causing splitting to occur and re- malloc extly what u freed: pointer(0), pointer(2) ...
         * once that is done , free all the nodes
         * we have the range 2-64, because if we get 1, 1-0 = 0 and thats an error
         */
        
        gettimeofday(&begin, NULL);
        
        int  size = (rand() % 63) +2;
        
        int capacity = (4997)/ (size +1) -1;
        
        void * pointers[capacity];
        
        for(i = 0; i<capacity; i++){
            
            pointers[i] = malloc(size);
        }
        
        for(i = 0; i<capacity; i+=2){
            
            free(pointers[i]);
        }
        int freeChunks = size - 1;
        
        for(i = 0; i<capacity; i+=2){
            
            pointers[i] = malloc (freeChunks);
        }
        for(i = 0; i<capacity; i++){
            
            free(pointers[i]);
        }
        
        gettimeofday(&end, NULL);
        elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
        e+= elapsed;
        
        /* F. Pick a random number of bytes 10 - 64 and malloc until you can no longer malloc
         * once we have mallocec as many as we can, free every other pointer: 0, 2 ,4 ....
         * now pick another random number that fits in the free space causing splitting to occur and re- malloc extly what u freed: pointer(0), pointer(2) ...
         * once that is done , free all the nodes
         */
        size = (rand() % 55) +10;
        
        capacity = (4997)/ (size +1) -1;
        
        void * pointers2[capacity];
        
        for(i = 0; i<capacity; i++){
            
            pointers2[i] = malloc(size);
        }
        
        for(i = 0; i<capacity; i+=2){
            
            free(pointers2[i]);
        }
        freeChunks = size -5;
        
        for(i = 0; i<capacity; i+=2){
            
            pointers2[i] = malloc (freeChunks);
        }
        for(i = 0; i<capacity; i++){
            
            free(pointers2[i]);
        }
        gettimeofday(&end, NULL);
        elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
        f += elapsed;
        
        count++;
    }
    
    printf("\nDone with test cases\n");
    printf("A: average time for 100 itterations took: %.10lf seconds\n", a/100);
    printf("B: average time for 100 itterations took: %.10lf seconds\n", b/100);
    printf("C: average time for 100 itterations took: %.10lf seconds\n", c/100);
    printf("D: average time for 100 itterations took: %.10lf seconds\n", d/100);
    printf("E: average time for 100 itterations took: %.10lf seconds\n", e/100);
    printf("F: average time for 100 itterations took: %.10lf seconds\n", f/100);
    
    char * endMessage = (char*) malloc(sizeof(char) * 40);
    
    endMessage = "The total time for all the test cases was: ";
    
    double totalTime = a + b + c + d + e + f;
    
    char * seconds = (char*) malloc(sizeof(char) * 9);
    
    seconds = " seconds";
    printf("\n%s%.10lf%s\n\n", endMessage, totalTime, seconds);
    printf("------------------------------------------------------------------------------------------------------------------\n");
    
    return 0;
}
