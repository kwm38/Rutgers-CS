/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

void readWrite (Instruction * instruction);
void workBackwords(Instruction * instruction, int field);
void deleteInstructions(Instruction * instruction);

void readWrite (Instruction * instruction){
    
    while(instruction != NULL){
        
        switch(instruction->opcode){
                
            case READ:
                
                instruction->critical = 'C';
                break;
                
            case WRITE:
                
                instruction->critical = 'C';
                workBackwords(instruction, instruction->field1);
                break;
                
            case STORE:
            case LOADI:
            case SUB:
            case MUL:
            case LOAD:
            case ADD:
                break;
                
        }
        
        instruction = instruction -> next;
    }
    return;
}

void workBackwords(Instruction * instruction, int field){
    
    Instruction * temp = instruction ->prev;
    
    while(temp!= NULL ){
        
        switch(temp->opcode){
                
            case ADD:
            case SUB:
            case MUL:
                
                if (temp->field1 == field) {
                    temp->critical = 'C';
                    workBackwords(temp, temp->field2);
                    workBackwords(temp, temp->field3);
                    return;
                }
                break;
                
            case LOADI:
                
                if (temp->field1 == field) {
                    temp->critical = 'C';
                    return;
                }
                
                break;
                
            case LOAD:
                
                if (temp->field1 == field) {
                    temp->critical = 'C';
                    workBackwords(temp, temp->field2);
                    return;
                }
                break;
                
            case STORE:
                
                if(temp->field1 == field){
                    
                    temp->critical = 'C';
                    workBackwords(temp, temp->field2);
                    return;
                    
                }
                break;
                
            case READ:
            case WRITE:
                break;
        }
        
        temp = temp->prev;
    }
}

void deleteInstructions(Instruction * instruction){
    
    Instruction * temp = instruction;
    
    while(temp!= NULL){
        
        if(temp->critical != 'C'){
            
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;
            Instruction * freeMe = temp;
            temp = temp->next;
            free(freeMe);
            continue;
            
        }
        temp = temp->next;
    }
}

int main(){
    
    Instruction *head;
    
    head = ReadInstructionList(stdin);
    
    Instruction *temp = head;
    
    if (!head) {
        WARNING("No instructions\n");
        exit(EXIT_FAILURE);
    }
    
    readWrite(temp);
    
    temp = head;
    
    deleteInstructions(temp);
    
    if (head) {
        PrintInstructionList(stdout, head);
        DestroyInstructionList(head);
    }
    return EXIT_SUCCESS;
}
