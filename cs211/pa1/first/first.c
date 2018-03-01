#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "first.h"

Node* removeNode( Node* head, int val)
{
	if(head == NULL){

		return head;
	}

	if(head->val == val){

		head = head->next;

		return head;

	}

	Node * mod = head;

	Node * prev = NULL;

	while(mod->next !=NULL && mod->val != val){

		prev=mod;

		mod = mod->next;

	}	

	if(mod->val != val){

		return head;
	}	

	prev->next = mod->next;


	return head;

}

Node* insert( Node* head, int val, int count)
{



	if(head == NULL){

		head = ( Node*) malloc(sizeof( Node));

		head->val = val;

		head->next = NULL; 

		return head;
	}
	Node * mod = head;

	Node * prev = NULL;

	while(mod !=NULL && mod->val <= val){

		if(mod->val == val){

			return head;

		}

		prev = mod;

		mod= mod->next;


	}
	Node* insert = ( Node*) malloc(sizeof( Node));

	insert->val  = val;

	insert->next = mod;

	if(prev != NULL){
		prev->next = insert;
	}else{

		head=insert;
	}
	return head;





}

int main(int argc, char **argv)
{
	struct Node* head = NULL;

	FILE *file;
	char * str = malloc(sizeof(char)* 20);
	int size=0;

	int count=0;


	file = fopen(argv[1], "r");


	if (file) {

		fseek (file, 0, SEEK_END);
		size =(int)ftell(file);

		if( size == 0){

			printf("%d\n", 0);
			printf("\n");
			return 0;            
		}else{

			rewind(file);

			while (fgets (str, sizeof(char)*20, file)){

				char opp= str[0];
				int val = atoi(&str[2]);



				if(opp == 'i'){


					head = insert(head, val, count);
					count = count +1;

				}else if(opp =='d'){

					head =  removeNode(head, val);


				}

			}
			fclose(file);


		}
	}else{


		printf("%s\n", "error");
		return 0;
	}

	count = 0;
	Node * temp = head;

	while(temp!=NULL){
		count++;
		temp=temp->next;

	}

	printf("%d\n", count);

	while(head!=NULL){

		printf("%d\t", head->val);

		head=head->next;

	}
	printf("\n");    
	return 0;

}
