#include <stdio.h> 
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "third.h"
#define SIZE 10000

Node * hashArray[SIZE];

void insert(int key, int data){

	Node *head = (Node *)malloc(sizeof(Node));

	head->data=data;
	head->next=NULL;


	if(hashArray[key] == NULL){

		hashArray[key]= head;
		printf("inserted\n");
		return;

	}else{

		Node * temp = hashArray[key];

		while(temp!=NULL){

			if(temp->data == data){

				printf("duplicate\n");
				return;
			}

			temp=temp->next;

		}

		temp = hashArray[key];
		head->next=temp;
		hashArray[key] = head;
		printf("inserted\n");
		return ;

	}

}

void search(int key, int data){



	Node *  head = hashArray[key];
	Node * temp = head;

	if(head == NULL){

		printf("absent\n");
		return;

	}else{

		while(temp!=NULL){

			if(temp->data == data){

				printf("present\n");
				return;
			}
			temp=temp->next;
		}

	}

	printf("absent\n");
}

int main(int argc, char **argv)
{
	FILE *file;
	char * str = malloc(sizeof(char) * 20);
	int size=0;

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
				int key = abs(val % 10000);
				if(opp =='i'){
					insert(key,val);				
				}else if(opp=='s'){

					search(key,val);
				}
			}
			fclose(file);

		}

	}else{


		printf("%s\n", "error");
		return 0;
	}

	return 0;
};
