#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include"second.h"

long inputs,temps, outputs;
int i,count,count2,j,c;
char * garbage;
Instruction * instructions;
Variable ** inputVar;
Variable ** tempVar;
Variable ** outputVar;
Instruction ** sorter;
int counting;

FILE * circuit;

int main(int argc, char * argv[]){
    
    if (argc <2 ) {
        printf("Missing Circuit file\n");
        return 0;
    }
    circuit = fopen(argv[1], "r");
    
    if(circuit == NULL){
        printf("Circuit file is empty\n");
        return 0;
    }
    garbage = malloc (sizeof(char) * 13);
    
    initializeInputs();
    initializeOutPuts();
    
    while((fscanf(circuit, "%s", garbage) != EOF)){
        
        count ++;
    }
    count2= count;
    
    char input [300];
    rewind(circuit);
    fgets(input, 300,circuit);
    fgets(input, 300, circuit);
    
    
    countTemps();
    
   
    rewind(circuit);
    fgets(input, 300,circuit);
    fgets(input, 300, circuit);
    
    
    initializeTemps();
    
  rewind(circuit);
  fgets(input, 300,circuit);
  fgets(input, 300, circuit);
  initializeInstrudtions();

    fclose(circuit);
    sortInstructions();
    grayCode();

    
    return 0;
}
void initializeInputs(){
    
    fscanf(circuit,"%s", garbage);
    fscanf(circuit,"%ld", &inputs);
    
    
    inputVar = malloc(sizeof(Variable)* (inputs +1));
    for(i=0; i<inputs+1; i++){
        
         inputVar[i] = malloc(sizeof(Variable));
    }
    for(i=0; i<inputs; i++){
    fscanf(circuit,"%s", garbage);
        
        inputVar[i]->name = malloc(sizeof(char)*13);
        strcpy(inputVar[i]->name, garbage);
        inputVar[i]->val = 0;
    }
}

void initializeOutPuts(){
    
    fscanf(circuit,"%s", garbage);
    fscanf(circuit,"%ld", &outputs);
    
    outputVar = malloc(sizeof(Variable)* (outputs +1));
    for(i=0; i<outputs+1; i++){
        
        outputVar[i] = malloc(sizeof(Variable));
    }
    for(i=0; i<outputs; i++){
        fscanf(circuit,"%s", garbage);
        
        outputVar[i]->name = malloc(sizeof(char)*13);
        strcpy(outputVar[i]->name, garbage);
        outputVar[i]->val = 0;
    }
}
void countTemps(){
    
    for(i=0; i<count; i++){
        
        fscanf(circuit,"%s", garbage);
        
        if(garbage[0]>= 'a' && garbage[0]<= 'z'){
            
            temps ++;
        }
      
    }
    
    
}
    

void initializeTemps(){
    int place =0;
    
    tempVar = malloc(sizeof(Variable)* (temps +1));
    for(i=0; i<temps+1; i++){
        
        tempVar[i] = malloc(sizeof(Variable));
         tempVar[i]->name = malloc(sizeof(char)*13);
    }
    
    for(i=0; i<count; i++){
        
        count --;
        fscanf(circuit,"%s", garbage);
        
        if(strcmp(garbage,"NOT")==0) {
            fscanf(circuit,"%s", garbage);
            fscanf(circuit,"%s", garbage);
            count -=2;
            
            if(garbage[0]>= 'a' && garbage[0]<= 'z'){
                
                tempVar[place]->val =-1;
                strcpy(tempVar[place]->name, garbage);
                place++;
                
            }
        }
        if(strcmp(garbage,"AND")==0 || strcmp(garbage,"NAND")==0 || strcmp(garbage,"OR")==0 || strcmp(garbage,"NOR")==0 || strcmp(garbage,"XOR")==0){
            
            fscanf(circuit,"%s", garbage);
            fscanf(circuit,"%s", garbage);
            fscanf(circuit,"%s", garbage);
            count-=3;
            
            if(garbage[0]>= 'a' && garbage[0]<= 'z'){
                
                tempVar[place]->val =-1;
                strcpy(tempVar[place]->name, garbage);
                place++;
                
            }

            
        }
        
        if(strcmp(garbage,"MULTIPLEXER")==0){
            
            long totalOutputs =0;
            fscanf(circuit,"%ld", &totalOutputs);
            count --;
                long in =(long) log2 (totalOutputs);
            
            for(j=0; j< totalOutputs; j++){
                
                fscanf(circuit,"%s", garbage);
                count--;
            }
            
            for(j=0; j< in; j++){
                
                fscanf(circuit,"%s", garbage);
                count --;
                
            }
            
            fscanf(circuit,"%s", garbage);
            count --;
            
            if(garbage[0]>= 'a' && garbage[0]<= 'z'){
                
                tempVar[place]->val =-1;
                strcpy(tempVar[place]->name, garbage);
                place++;
                
            }
            
        }
        if(strcmp(garbage,"DECODER")==0){
            
            long totalIn =0;
            fscanf(circuit,"%ld", &totalIn);
            count --;
            long out = (1<<totalIn);
            
            for(j=0; j< totalIn; j++){
                
                fscanf(circuit,"%s", garbage);
                count--;
            }
            
            for(j=0; j< out; j++){
                
                fscanf(circuit,"%s", garbage);
                count --;
                
                if(garbage[0]>= 'a' && garbage[0]<= 'z'){
                    
                    tempVar[place]->val =-1;
                    strcpy(tempVar[place]->name, garbage);
                    place++;
                    
                }
                
            }
            
        }
        i=-1;
    }
    int bing=0;
    for(i=0; i< temps; i++){
        
        if(!(strcmp(tempVar[i]->name,"")==0)){
            bing ++;
        }
    }
    temps = bing;
    
}

    void initializeInstrudtions(){
        
        Instruction * temp=malloc(sizeof(Instruction));
        instructions= temp;
        
        for(i=0; i<count2; i++){
            
            
            fscanf(circuit,"%s", garbage);
            count2 --;
            
            if(strcmp(garbage,"NOT")==0) {
                
                temp->op = malloc(sizeof(char)*13);
                strcpy(temp->op, "NOT");
                temp->nextInstruction=NULL;
               
                temp->inputList = malloc(sizeof(Variable)*2);
                temp-> inputList[0] = malloc(sizeof(Variable));
                
                temp->outPutList = malloc(sizeof(Variable)*2);
                temp-> outPutList[0] = malloc(sizeof(Variable));
                
                fscanf(circuit,"%s", garbage);
                
                if(strlen(garbage)==1){
                    
                    Variable * number = malloc(sizeof(Variable));
                    number->name=malloc(sizeof(char)*13);
                    strcpy(number->name, "num");
                    number->val = atoi(&garbage[0]);
                    temp-> inputList[0] =number;
              
                }else if(!(garbage[0]>= 'a' && garbage[0]<= 'z')){
                    
                    for(j=0; j<inputs; j++){
                        
                        if(strcmp(inputVar[j]->name,garbage)==0){
                        
                             temp-> inputList[0] =inputVar[j];
                            break;
                        }
                    }
                    
                }else{
                    
                    for(j=0; j<temps; j++){
                        
                        if(strcmp(tempVar[j]->name,garbage)==0){
                            
                            temp-> inputList[0] =tempVar[j];
                            break;
                        }
                    }
                }
                
                fscanf(circuit,"%s", garbage);
                count2 -=2;
                if(!(garbage[0]>= 'a' && garbage[0]<= 'z')){
                    
                    for(j=0; j<outputs; j++){
                        
                        if(strcmp(outputVar[j]->name,garbage)==0){
                            
                            temp-> outPutList[0] =outputVar[j];
                            break;
                        }
                    }
                    
                }else{
                    
                    for(j=0; i<temps; j++){
                    
                        if(strcmp(tempVar[j]->name,garbage)==0){
                            
                            temp-> outPutList[0] =tempVar[j];
                            break;
                        }
                    }
                }
                if(count2!=0){
                    temp->nextInstruction=malloc(sizeof(Instruction));
                    temp = temp->nextInstruction;
                    
                    
                }else{
                    temp->nextInstruction= NULL;
                }
            }
             if(strcmp(garbage,"AND")==0 || strcmp(garbage,"NAND")==0 || strcmp(garbage,"OR")==0 || strcmp(garbage,"NOR")==0 || strcmp(garbage,"XOR")==0) {
                
                temp->op = malloc(sizeof(char)*13);
                strcpy(temp->op, garbage);
                temp->nextInstruction=NULL;
                
                temp->inputList = malloc(sizeof(Variable)*2);
                temp-> inputList[0] = malloc(sizeof(Variable));
                temp-> inputList[1] = malloc(sizeof(Variable));
                
                temp->outPutList = malloc(sizeof(Variable)*2);
                temp-> outPutList[0] = malloc(sizeof(Variable));
                
                
                
                 for(j=0; j<2; j++){
               fscanf(circuit,"%s", garbage);
                if(strlen(garbage)==1){
                    
                    Variable * number = malloc(sizeof(Variable));
                    number->name=malloc(sizeof(char)*13);
                    strcpy(number->name, "num");
                    number->val = atoi(&garbage[0]);
                    temp-> inputList[j] =number;
                    
                }else if(!(garbage[0]>= 'a' && garbage[0]<= 'z')){
                    
                    for(c=0; c<inputs; c++){
                        
                        if(strcmp(inputVar[c]->name,garbage)==0){
                            
                            temp-> inputList[j] =inputVar[c];
                            break;
                        }
                    }
                    
                }else{
                    
                    for(c=0; c<temps; c++){
                        
                        if(strcmp(tempVar[c]->name,garbage)==0){
                            
                            temp-> inputList[j] =tempVar[c];
                            break;
                        }
                    }
                }
            }
                
                fscanf(circuit,"%s", garbage);
                count2 -=3;
                if(!(garbage[0]>= 'a' && garbage[0]<= 'z')){
                    
                    for(j=0; j<outputs; j++){
                        
                        if(strcmp(outputVar[j]->name,garbage)==0){
                            
                            temp-> outPutList[0] =outputVar[j];
                            break;
                        }
                    }
                    
                }else{
                    
                    for(j=0; j<temps; j++){
                        
                        if(strcmp(tempVar[j]->name,garbage)==0){
                            
                            temp-> outPutList[0] =tempVar[j];
                            break;
                        }
                    }
                }
                if(count2!=0){
                    temp->nextInstruction=malloc(sizeof(Instruction));
                    temp = temp->nextInstruction;
                    
                    
                }else{
                    temp->nextInstruction= NULL;
                }
            }
            if(strcmp(garbage,"MULTIPLEXER")==0){
                
                temp->op = malloc(sizeof(char)*13);
                strcpy(temp->op, garbage);
                temp->nextInstruction=NULL;
                
                long totalOutputs =0;
                fscanf(circuit,"%ld", &totalOutputs);
                count2 --;
                long in =(long) log2(totalOutputs);
            
                temp->inNum = in;
                temp->outNum =totalOutputs;
                
                temp->inputList = malloc(sizeof(Variable)*totalOutputs+1);
                for(i=0; i<totalOutputs;i++){
                    
                    temp-> inputList[i] = malloc(sizeof(Variable));
                }
                temp->selector = malloc(sizeof(Variable)*in+1);
                for(i=0; i<in;i++){
                    
                    temp-> selector[i] = malloc(sizeof(Variable));
                }
                
                temp->outPutList = malloc(sizeof(Variable)*2);
                temp-> outPutList[0] = malloc(sizeof(Variable));
                
                
                for(j=0; j<totalOutputs;j++){
                    
                    fscanf(circuit,"%s", garbage);
                    if(strlen(garbage)==1){
                        
                        Variable * number = malloc(sizeof(Variable));
                        number->name=malloc(sizeof(char)*13);
                        strcpy(number->name, "num");
                        number->val = atoi(&garbage[0]);
                        temp-> inputList[j] =number;
                        
                    }else if(!(garbage[0]>= 'a' && garbage[0]<= 'z')){
                        
                        for(c=0; c<inputs; c++){
                            
                            if(strcmp(inputVar[c]->name,garbage)==0){
                                
                                temp-> inputList[j] =inputVar[c];
                                break;
                            }
                        }
                        
                    }else{
                        
                        for(c=0; c<temps; c++){
                            
                            if(strcmp(tempVar[c]->name,garbage)==0){
                                
                                temp-> inputList[j] =tempVar[c];
                                break;
                            }
                        }
                    }
                    
            }
                
                count2-=totalOutputs;
                count2-=in;
                
                for(j=0; j<in;j++){
                    
                    fscanf(circuit,"%s", garbage);
                    if(strlen(garbage)==1){
                        
                        Variable * number = malloc(sizeof(Variable));
                        number->name=malloc(sizeof(char)*13);
                        strcpy(number->name, "num");
                        number->val = atoi(&garbage[0]);
                        temp-> selector[j] =number;
                        
                    }else if(!(garbage[0]>= 'a' && garbage[0]<= 'z')){
                        
                        for(c=0; c<inputs; c++){
                            
                            if(strcmp(inputVar[c]->name,garbage)==0){
                                
                                temp-> selector[j] =inputVar[c];
                                break;
                            }
                        }
                        
                    }else{
                        
                        for(c=0; c<temps; c++){
                            
                            if(strcmp(tempVar[c]->name,garbage)==0){
                                
                                temp-> selector[j] =tempVar[c];
                                break;
                            }
                        }
                    }
                    
                }
                    
                fscanf(circuit,"%s", garbage);
                count2 --;
                if(!(garbage[0]>= 'a' && garbage[0]<= 'z')){
                    
                    for(j=0; j<outputs; j++){
                        
                        if(strcmp(outputVar[j]->name,garbage)==0){
                            
                            temp-> outPutList[0] =outputVar[j];
                            break;
                        }
                    }
                    
                }else{
                    
                    for(j=0; j<temps; j++){
                        
                        if(strcmp(tempVar[j]->name,garbage)==0){
                            
                            temp-> outPutList[0] =tempVar[j];
                            break;
                        }
                    }
                }
                
                if(count2!=0){
                    temp->nextInstruction=malloc(sizeof(Instruction));
                    temp = temp->nextInstruction;
                    
                }else{
                    
                    temp->nextInstruction= NULL;
                }
                
            }
            
            if(strcmp(garbage,"DECODER")==0){
                
                temp->op = malloc(sizeof(char)*13);
                strcpy(temp->op, garbage);
                temp->nextInstruction=NULL;
                
                long totalIn =0;
                fscanf(circuit,"%ld", &totalIn);
                count2 --;
                
                long out = (1<<totalIn);
                
                temp->inNum = totalIn;
                temp->outNum =out;
                
                temp->inputList = malloc(sizeof(Variable)*totalIn+1);
                for(i=0; i<totalIn;i++){
                    
                    temp-> inputList[i] = malloc(sizeof(Variable));
                }
                temp->outPutList = malloc(sizeof(Variable)*out+1);
                for(i=0; i<out;i++){
                    
                    temp-> outPutList[i] = malloc(sizeof(Variable));
                }
                
                
                for(j=0; j< totalIn; j++){
                    
                    fscanf(circuit,"%s", garbage);
                    count2--;
                    if(strlen(garbage)==1){
                        
                        Variable * number = malloc(sizeof(Variable));
                        number->name=malloc(sizeof(char)*13);
                        strcpy(number->name, "num");
                        number->val = atoi(&garbage[0]);
                        temp-> inputList[j] =number;
                        
                    }else if(!(garbage[0]>= 'a' && garbage[0]<= 'z')){
                        
                        for(c=0; c<inputs; c++){
                            
                            if(strcmp(inputVar[c]->name,garbage)==0){
                                
                                temp-> inputList[j] =inputVar[c];
                                break;
                            }
                        }
                        
                    }else{
                        
                        for(c=0; c<temps; c++){
                            
                            if(strcmp(tempVar[c]->name,garbage)==0){
                                
                                temp-> inputList[j] =tempVar[c];
                                break;
                            }
                        }
                    }
                }
                
                for(j=0; j< out; j++){
                    
                    fscanf(circuit,"%s", garbage);
                    count2--;
                    
                    if(!(garbage[0]>= 'a' && garbage[0]<= 'z')){
                        
                        for(c=0; c<outputs; c++){
                            
                            if(strcmp(outputVar[c]->name,garbage)==0){
                                
                                temp-> outPutList[j] =outputVar[c];
                                break;
                            }
                        }
                        
                    }else{
                        
                        for(c=0; c<temps; c++){
                            
                            if(strcmp(tempVar[c]->name,garbage)==0){
                                
                                temp-> outPutList[j] =tempVar[c];
                                break;
                            }
                        }
                    }
                }
                
                if(count2!=0){
                    temp->nextInstruction=malloc(sizeof(Instruction));
                    temp = temp->nextInstruction;
                    
                }else{
                    
                    temp->nextInstruction= NULL;
                }
            }
            
            i=-1;
        }
        
    }
    


void grayCode(){
    
    long grayCode=0;
    
    for (i=0; i<((1<<(inputs))); i++){
        
        grayCode= i ^ ((i) >> 1);
        
        followInstructions(grayCode);
    }
}

void followInstructions( long grayCode){
    int c;
    int grayCodeArray[inputs];
    unsigned long   mask = 1<<(inputs-1);
    int count =0;
    
    while(mask > 0) {
        if((grayCode & mask) == 0 )
            grayCodeArray[count] = 0;
        else
            grayCodeArray[count] = 1;
        mask = mask >> 1 ;
        count++;
        
    }
    
    for(c=0; c<inputs; c++){
        
        inputVar[c]->val=grayCodeArray[c];
        
    }
    
    int kl;
    for(kl=0; kl< temps; kl++){
        
        tempVar[kl]->val =-1;
        
    }
    int m;
    int complete=0;
    
    int jk =0;
    
    for(jk =0; jk<temps; jk++){
        
        tempVar[jk]->val =-1;
        tempVar[jk]->done =0;
        
    }
    for(jk =0; jk<outputs; jk++){
        
        outputVar[jk]->done =0;
    
    }for(jk =0; jk<inputs; jk++){
        
       inputVar[jk]->done =0;
        
    }
    while(complete< counting){
       
    for(m=0; m<counting; m++){
        
        
        if(strcmp(sorter[m]->op,"NOT")==0){
            if( (sorter[m]->outPutList[0]->name [0]>= 'a' &&  sorter[m]->outPutList[0]->name[0]<= 'z') && sorter[m]->outPutList[0]->done ==1){
                
                continue;
            }
          
            if(sorter[m]->inputList[0]->val != -1){
                
            sorter[m]->outPutList[0]->val = ~ sorter[m]->inputList[0]->val &1;
                
           
                complete ++;
            
                 sorter[m]->outPutList[0]->done =1;
                 continue;
            }
            
            
        }else if(strcmp(sorter[m]->op,"AND")==0){
             if( (sorter[m]->outPutList[0]->name [0]>= 'a' &&  sorter[m]->outPutList[0]->name[0]<= 'z') && sorter[m]->outPutList[0]->done ==1){
               
                continue;
            }
               if(sorter[m]->inputList[0]->val != -1 && sorter[m]->inputList[1]->val !=-1){
                   
                   sorter[m]->outPutList[0]->val = sorter[m]->inputList[0]->val & sorter[m]->inputList[1]->val;
             
                   
                  
                       complete ++;
                   
                   sorter[m]->outPutList[0]->done =1;
                    continue;
                   
               }
            
           
          
        }else if(strcmp(sorter[m]->op,"NAND")==0 ){
            if( (sorter[m]->outPutList[0]->name [0]>= 'a' &&  sorter[m]->outPutList[0]->name[0]<= 'z') && sorter[m]->outPutList[0]->done ==1){
                
                continue;
            }
             if(sorter[m]->inputList[0]->val != -1 && sorter[m]->inputList[1]->val !=-1){
                 
            sorter[m]->outPutList[0]->val =  sorter[m]->inputList[0]->val & sorter[m]->inputList[1]->val;
                 
            sorter[m]->outPutList[0]->val =  ~  sorter[m]->outPutList[0]->val &1;
                 
                 
                     
                     complete ++;
                     
                 
                 sorter[m]->outPutList[0]->done =1;
                 continue;
             }

            
        }else if(strcmp(sorter[m]->op,"OR")==0 ){
             if( (sorter[m]->outPutList[0]->name [0]>= 'a' &&  sorter[m]->outPutList[0]->name[0]<= 'z') && sorter[m]->outPutList[0]->done ==1){
                
                continue;
            }
             if(sorter[m]->inputList[0]->val != -1 && sorter[m]->inputList[1]->val !=-1){
                 
           sorter[m]->outPutList[0]->val =  sorter[m]->inputList[0]->val | sorter[m]->inputList[1]->val;
            
                 
                     
                     complete ++;
                     
                 
                  sorter[m]->outPutList[0]->done =1;
                 continue;
             }
        }else if(strcmp(sorter[m]->op,"NOR")==0 ){
           if( (sorter[m]->outPutList[0]->name [0]>= 'a' &&  sorter[m]->outPutList[0]->name[0]<= 'z') && sorter[m]->outPutList[0]->done ==1){
                
                continue;
            }
             if(sorter[m]->inputList[0]->val != -1 && sorter[m]->inputList[1]->val !=-1){
            sorter[m]->outPutList[0]->val =  sorter[m]->inputList[0]->val | sorter[m]->inputList[1]->val;
            sorter[m]->outPutList[0]->val=  ~  sorter[m]->outPutList[0]->val &1;
                
                     complete ++;
                 
                  sorter[m]->outPutList[0]->done =1;
                 continue;
             }
           
        }else if(strcmp(sorter[m]->op,"XOR")==0){
             if( (sorter[m]->outPutList[0]->name [0]>= 'a' &&  sorter[m]->outPutList[0]->name[0]<= 'z') && sorter[m]->outPutList[0]->done ==1){
                
                continue;
            }
             if(sorter[m]->inputList[0]->val != -1 && sorter[m]->inputList[1]->val !=-1){
            sorter[m]->outPutList[0]->val =  sorter[m]->inputList[0]->val ^ sorter[m]->inputList[1]->val;
                 
                 
                 
                     complete ++;
                  sorter[m]->outPutList[0]->done =1;
                 continue;
             }
            
        }else if (strcmp(sorter[m]->op,"MULTIPLEXER")==0){
            if( (sorter[m]->outPutList[0]->name [0]>= 'a' &&  sorter[m]->outPutList[0]->name[0]<= 'z') && sorter[m]->outPutList[0]->done ==1){
             
                continue;
            }
            
            int z;
            int stop =0;
            for(z=0;z<sorter[m]->outNum; z++){
                
                
                if(sorter[m]->inputList[z]->val == -1){
                    stop=1;
                  
                    break;
                }
                
                
            }
            for(z=0;z<sorter[m]->inNum; z++){
                
                
                if(sorter[m]->selector[z]->val == -1){
                    stop=1;
                   
                    break;
                }
                
                
            }
        
            if(stop ==1){
           
                stop =0;
                
                continue;
            }
       
            int k;
            unsigned long bin =0;
            int count =0;
            for(k=sorter[m]->inNum-1; k>=0; k--){
                bin += sorter[m]->selector[k]->val<<count;
                count ++;
            }
            
            unsigned long grayCode =0;
            int x=0;
            int g=0;
            for (g=0; g<sorter[m]->outNum; g++){
                grayCode = g ^ (g >> 1);
                if(grayCode == bin){
                    x=g;
                    break;
                }
            }
           
            sorter[m]->outPutList[0]->val =  sorter[m]->inputList[x]->val;
            
           
                
                complete ++;
            
             sorter[m]->outPutList[0]->done =1;
            continue;
            
        }else if (strcmp(sorter[m]->op,"DECODER")==0){
            
            
            int z;
            int stop =0;
            for(z=0; z<sorter[m]->inNum; z++){
                
                if(sorter[m]->inputList[z]->val == -1){
                    stop =1;
                    break;
                }
                
            }
            if(stop ==1){
                
                stop =0;
                
                continue;
            }
            
            
            int k;
            unsigned long bin =0;
            count =0;
            for(k=sorter[m]->inNum-1; k>=0; k--){
                bin += sorter[m]->inputList[k]->val<<count;
                count ++;
            }
            
            unsigned long grayCode =0;
            int x=0;
            int g=0;
            for (g=0; g<sorter[m]->outNum; g++){
                grayCode = g ^ (g >> 1);
                if(grayCode == bin){
                    x=g;
                    break;
                }
            }
            
            for(c=0;c<sorter[m]->outNum; c++){

               if (c==x) {
                 
                   sorter[m]->outPutList[c]->val=1;
               
                   
                   sorter[m]->outPutList[c]->done =1;
               }else{
                   
                   sorter[m]->outPutList[c]->val=0;
                   
                   
                 sorter[m]->outPutList[c]->done =1;
               }
          }
            complete ++;
        }
       
    }
        m=0;
      
}
  
    for (c = 0; c <inputs; c++) {
        
        printf("%d ",grayCodeArray[c]);
        
    }
    int h;
    for (h = 0; h <outputs; h++) {
        
        printf("%d ",outputVar[h] ->val);
    }
    printf("\n");
    
}

void sortInstructions(){
    
    Instruction * count = instructions;
    
    int counter =0;
    
    while(count!=NULL){
        
        counter ++;
        count = count->nextInstruction;
        
    }
    
    sorter= malloc(sizeof(Instruction)* counter +1);
    
     Instruction * grab = instructions;
    
    for(i=0; i < counter; i++){
        
        sorter[i]= malloc(sizeof(Instruction) *2 );
        sorter[i]= grab;

        grab = grab->nextInstruction;
        
    }

    counting = counter;
  
    
}

    
    
    
    
    
    


