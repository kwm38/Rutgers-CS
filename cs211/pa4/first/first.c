
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include"first.h"

int cacheSize,blockSize,assoc,sets,blockOffset,setIndex;
FILE * traceFile;
Line ** cacheNoPre;
Line ** cachePre;
int i,j;
int read, write, miss, hit,readP, writeP, missP, hitP;
char * policy;
long itteration;

int main(int argc, char * argv[]){
    
    if(argc != 6){
        printf("Not Enough arguements");
        exit(0);
    }
    traceFile= fopen(argv[5], "r");
    
    if(traceFile == NULL){
        printf("Empty file");
        exit(0);
    }
   
    if(strcmp("fifo",argv[3]) == 0){
        
        policy = "fifo";
        
    }else{
        
        policy ="lru";
    }
    
    cacheSize = (int) atoi(argv[1]);
    blockSize = (int) atoi(argv[4]);
    
    
    if(strcmp("direct",argv[2]) == 0){
        
        assoc = 1;
        cacheSize = atoi(argv[1]);
        blockSize = atoi(argv[4]);
        sets = cacheSize / blockSize;
        
    }else if(strcmp("assoc",argv[2]) == 0){
        
        cacheSize = atoi(argv[1]);
        blockSize = atoi(argv[4]);
        assoc = cacheSize / blockSize;
        sets = 1;
        
    }else{
        assoc = (int) atoi(&argv[2][6]);
        
        cacheSize = atoi(argv[1]);
        blockSize = atoi(argv[4]);
        sets = cacheSize / (blockSize * assoc);
        
    }
    
    initialize();
    readfile();
    clearMem();
    
    fclose(traceFile);
    printf("no-prefetch\n");
    printf("Memory reads: %d\n",read);
    printf("Memory writes: %d\n", write);
    printf("Cache hits: %d\n", hit);
    printf("Cache misses: %d\n",miss);
    
    printf("with-prefetch\n");
    printf("Memory reads: %d\n",readP);
    printf("Memory writes: %d\n", writeP);
    printf("Cache hits: %d\n", hitP);
    printf("Cache misses: %d\n",missP);
    return 0;
}
void initialize(){
    
    
    readP=writeP=hitP=missP=read=write=hit=miss=0;
    
    blockOffset = log2(blockSize);
    
    setIndex = log2(sets);
    
    cacheNoPre= (Line**) malloc(sets* sizeof(Line*));
    cachePre= (Line**) malloc(sets* sizeof(Line*));
    
    for(i =0; i<sets; i++){
        cacheNoPre[i]= (Line*) malloc(assoc * sizeof(Line));
        cachePre[i]= (Line*) malloc(assoc * sizeof(Line));
    }
    for(i=0; i < sets; i++){
        
        for(j=0; j < assoc; j++){
            
            cacheNoPre[i][j].tag=0;
            cacheNoPre[i][j].valid=0;
            cacheNoPre[i][j].order=0;
            cacheNoPre[i][j].timestamp=0;
            cachePre[i][j].tag=0;
            cachePre[i][j].valid=0;
            cachePre[i][j].order=0;
            cachePre[i][j].timestamp=0;
            
        }
        
    }
}

void readfile(){
    
    char oper;
    char counter[100];
    char address[20];
    long  data;
    data=0;
    long tag=0;
    int index=0;
    
    
    while((fscanf(traceFile, "%s %c %s\n", counter, &oper, address) != EOF) && (strcmp(counter, "#eof") != 0)){
        
        itteration++;
        
        data =(long)strtol(address,NULL,16);
        
        tag = (data>>(setIndex+blockOffset));
        
        index = (data >>(blockOffset)) & ((1 << setIndex) -1);
        
        
        
        if(oper == 'R'){
            
            if (opperate(tag, index, cacheNoPre, 0) == 0){
                
                miss++;
                read++;
                
            }else{
                hit++;
            }
            
            if (opperate(tag, index, cachePre, 0) == 0){
                
                missP++;
                readP++;
                
                tag = ((data + blockSize) >>(setIndex+blockOffset));
                index = ((data+ blockSize) >>(blockOffset)) & ((1 << setIndex) -1);
                if(opperate(tag, index, cachePre, 1)==0){
                    
                    readP++;
                }
                
            }else{
                hitP++;
            }
            
        }else if (oper == 'W'){
            
            if (opperate(tag, index, cacheNoPre, 0) == 0){
                miss++;
                write++;
                read++;
                
            }else{
                hit++;
                write++;
            }
            
            if (opperate(tag, index,cachePre, 0) == 0){
                missP++;
                writeP++;
                readP++;
                
                tag = ((data + blockSize) >>(setIndex+blockOffset));
                index = ((data+ blockSize) >>(blockOffset)) & ((1 << setIndex) -1);
                if(opperate(tag, index, cachePre, 1)==0){
                    
                    readP++;
                }
                
            }else{
                hitP++;
                writeP++;
            }
            
            
        }
    }
    
}

int opperate(long tag, int index,Line ** cache, int pf ){
    

    for(i=0 ; i< assoc; i++){
        
        if(cache[index][i].tag == tag){
            if(pf==0){
                
                cache[index][i].timestamp=itteration;
            }
           return 1;
        }
        
    }
    

    for(i=0 ; i< assoc; i++){
        
        if(cache[index][i].valid == 0){
            
            cache[index][i].valid =1;
            cache[index][i].tag = tag;
            cache[index][i].order =i+1;
            cache[index][i].timestamp =itteration;

            return 0;
            
        }
        
    }
    

    
    if(strcmp("fifo",policy) == 0){
        
        for(i=0 ; i< assoc; i++){
            
            
            if(cache[index][i].order == 1){
                
                break;
            }
            
        }
        cache[index][i].valid =1;
        cache[index][i].tag = tag;
        cache[index][i].order =assoc+1;
        cache[index][i].timestamp=itteration;
    
        
    }
    
    
    if(strcmp("lru",policy) == 0){
        
        int oldest =0;
        int oldestAge=cache[index][0].timestamp;
        
        for(i=1 ; i< assoc; i++){
          
            if(cache[index][i].timestamp < oldestAge){
               
                oldestAge=cache[index][i].timestamp;
                oldest=i;
                
            }
            
        }
        
        cache[index][oldest].valid =1;
        cache[index][oldest].tag = tag;
        cache[index][oldest].order =assoc+1;
        cache[index][oldest].timestamp =itteration;
        
    }
 
   
    for(j=0 ; j< assoc; j++){
        
        cache[index][j].order--;
    }
    
    return 0;
}

void clearMem(){
    
    
    for(i =0; i<sets; i++){
        free(cacheNoPre[i]);
        free(cachePre[i]);
    }
    
    free(cacheNoPre);
    free(cachePre);
    
}





