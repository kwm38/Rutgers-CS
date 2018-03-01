#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include"first.h"

long inputs,temps, outputs;
int i,count,count2,j,c;
char * garbage;
Instruction * instructions;
Variable ** inputVar;
Variable ** tempVar;
Variable ** outputVar;


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
    
    initializeInputs();//get inputvars
    initializeOutPuts();//get output vars
    
    while((fscanf(circuit, "%s", garbage) != EOF)){
        
        count ++;
    }
    count2= count;
    
    char input [300];
    rewind(circuit);
    fgets(input, 300,circuit);
    fgets(input, 300, circuit);
    
    
    countTemps();//get temp coubnt
    
   
    rewind(circuit);
    fgets(input, 300,circuit);
    fgets(input, 300, circuit);
    
    
    initializeTemps();// get temps
    
  rewind(circuit);
  fgets(input, 300,circuit);
  fgets(input, 300, circuit);
  initializeInstrudtions();

    fclose(circuit); //close the file for good
    
    grayCode();
 /*
    for(i=0; i<inputs; i++){
        
        printf("%s\n",inputVar[i] ->name);
    }
    for(i=0; i<outputs; i++){
        
         printf("%s\n",outputVar[i] ->name);
    }
    for(i=0; i<temps; i++){
        
        printf("%s\n",tempVar[i] ->name);
    }
    
    while(instructions!=NULL){
        
        printf("%s: ",instructions->op);
        printf("%s: ",instructions->outPutList[0]->name);
        instructions=instructions->nextInstruction;
    }*/
    
    return 0;
}
void initializeInputs(){
    
    fscanf(circuit,"%s", garbage); //INPUTVAR
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
    
    fscanf(circuit,"%s", garbage); //OUTPUTVAR
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
      
    }//for loop
    
    
}//countTemps
    

void initializeTemps(){
    
    tempVar = malloc(sizeof(Variable)* (temps +1));
    for(i=0; i<temps+1; i++){
        
        tempVar[i] = malloc(sizeof(Variable));
    }
    
    int place =0;
    for(i=0; i<count; i++){
        
        fscanf(circuit,"%s", garbage);
        
        if(garbage[0]>= 'a' && garbage[0]<= 'z'){
            
            tempVar[place]->name = malloc(sizeof(char)*13);
            strcpy(tempVar[place]->name, garbage);
            tempVar[place]->val = 0;
            place++;
        }
    }
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
                
                temp->inputList = malloc(sizeof(Variable)*totalOutputs+1);//outs
                for(i=0; i<totalOutputs;i++){
                    
                    temp-> inputList[i] = malloc(sizeof(Variable));
                }
                temp->selector = malloc(sizeof(Variable)*in+1);//selectors
                for(i=0; i<in;i++){
                    
                    temp-> selector[i] = malloc(sizeof(Variable));
                }
                
                temp->outPutList = malloc(sizeof(Variable)*2);//final output
                temp-> outPutList[0] = malloc(sizeof(Variable));
                
                
                for(j=0; j<totalOutputs;j++){ //get all outs
                    
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
                
                for(j=0; j<in;j++){ //get all selector
                    
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
                
                temp->inputList = malloc(sizeof(Variable)*totalIn+1);//outs
                for(i=0; i<totalIn;i++){
                    
                    temp-> inputList[i] = malloc(sizeof(Variable));
                }
                temp->outPutList = malloc(sizeof(Variable)*out+1);//outputs
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
        }//for loop
        
    }//end of initialize instructions
    


void grayCode(){
    
    long grayCode=0;
    
    for (i=0; i<((1<<(inputs))); i++){
        
        grayCode= i ^ ((i) >> 1);
        
        followInstructions(grayCode);
    }
}//end of graycode

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
    
    Instruction * run = instructions;
    
    while(run!=NULL){
        
        
        if(strcmp(run->op,"NOT")==0){
            
            run->outPutList[0]->val = ~ run->inputList[0]->val &1;
            run = run->nextInstruction;
            continue;
            
        }else if(strcmp(run->op,"AND")==0){
            
           run->outPutList[0]->val = run->inputList[0]->val & run->inputList[1]->val;
            run = run->nextInstruction;
            continue;
            
        }else if(strcmp(run->op,"NAND")==0 ){
            
            run->outPutList[0]->val =  run->inputList[0]->val & run->inputList[1]->val;
            run->outPutList[0]->val =  ~  run->outPutList[0]->val &1;
            run = run->nextInstruction;
            continue;
            
        }else if(strcmp(run->op,"OR")==0 ){
            
           run->outPutList[0]->val =  run->inputList[0]->val | run->inputList[1]->val;
            run = run->nextInstruction;
            continue;
            
        }else if(strcmp(run->op,"NOR")==0 ){
            
            run->outPutList[0]->val =  run->inputList[0]->val | run->inputList[1]->val;
            run->outPutList[0]->val=  ~  run->outPutList[0]->val &1;
            run = run->nextInstruction;
            continue;
            
        }else if(strcmp(run->op,"XOR")==0){
            
            run->outPutList[0]->val =  run->inputList[0]->val ^ run->inputList[1]->val;
            run = run->nextInstruction;
            continue;
            
        }else if (strcmp(run->op,"MULTIPLEXER")==0){
            
            int k;
            unsigned long bin =0;
            int count =0;
            for(k=run->inNum-1; k>=0; k--){
                bin += run->selector[k]->val<<count;
                count ++;
            }
            
            //printf("bin %ld ", bin);
            
            unsigned long grayCode =0;
            int x=0;
            int g=0;
            for (g=0; g<run->outNum; g++){
                grayCode = g ^ (g >> 1);
                if(grayCode == bin){
                    x=g;
                    break;
                }
            }
           
            run->outPutList[0]->val =  run->inputList[x]->val;
            
            run = run->nextInstruction;
            continue;
            
        }else if (strcmp(run->op,"DECODER")==0){
            
            int k;
            unsigned long bin =0;
            count =0;
            for(k=run->inNum-1; k>=0; k--){
                bin += run->inputList[k]->val<<count;
                count ++;
            }
            
            unsigned long grayCode =0;
            int x=0;
            int g=0;
            for (g=0; g<run->outNum; g++){
                grayCode = g ^ (g >> 1);
                if(grayCode == bin){
                    x=g;
                    break;
                }
            }
            
            for(c=0;c<run->outNum; c++){

               if (c==x) {
                 
                   run->outPutList[c]->val=1;
                   
               }else{
                   
                   run->outPutList[c]->val=0;
                
               }
          }
       run = run->nextInstruction;
        }//end of decoder
       
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

    
    
    
    
    
    

