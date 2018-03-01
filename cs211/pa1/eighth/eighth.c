#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "eighth.h"

struct Node* root;
int row;


void clearSpace(Node *head){

	if(head==NULL){

		return;

	}else if(head->left == NULL && head->right==NULL){

		free(head);
		return;
	}else{


		clearSpace(head->left);
		clearSpace(head->right);
		free(head);
	}


	return;


}
void insert(Node * head, int val){



	if(root == NULL){

		root = (Node*)malloc(sizeof(Node));

		root->key=val;
		root->less_eq=1;
		root->left=NULL;
		root->right=NULL;
		printf("inserted %d\n",row);
		row=1;
		return;

	}else{


		if(val == head->key){

			printf("duplicate \n");
			row=1;
			return;

		}else if(val < head->key){

			if(head->left == NULL){

				head->less_eq +=1;

				Node* temp= (Node*)malloc(sizeof(Node));

				temp->key=val;
				temp->left=NULL;
				temp->right=NULL;
				temp->less_eq=1;
				head->left = temp;
				printf("inserted %d\n",row +1 );
				row =1;
				return;
			}else{
				row++;
				insert(head->left, val);
			}

		}else{


			if(head ->right == NULL){


				Node * temp= (Node*)malloc(sizeof(Node));

				temp->key=val;
				temp->left=NULL;
				temp->right=NULL;
				head->right = temp;
				printf("inserted %d\n",row +1);
				row=1;
				return;
			}else{
				row++;
				insert(head->right, val);
			}



		}

	}
	row=1;
	return;
}



void search (Node* head, int val){

	if(head ==NULL){


		printf("absent\n");
		return;

	}

	if(head->key == val){

		printf("present %d\n", row);
		row=1;
		return;
	}else if(val < head->key){

		if(head->left == NULL){

			printf("absent\n");
			row=1;
			return;

		}else{
			row++;
			search(head->left, val);


		}



	}else{

		if(head->right == NULL){

			row=1;
			printf("absent\n");
			return;

		}else{
			row++;
			search(head->right, val);


		}


	}
	row=1;
	return;
}



int main(int argc, char ** argv){

	root = NULL;
	row =1;
	FILE *file;
	char * str = malloc(sizeof(char)* 20);
	int size=0;

	file = fopen(argv[1], "r");


	if (file) {

		fseek (file, 0, SEEK_END);
		size =(int)ftell(file);

		if( size == 0){

			printf("error\n");
			return 0;
		}else{

			rewind(file);

			while (fgets (str, sizeof(char)*20, file)){

				char opp= str[0];
				int val = atoi(&str[2]);

				if(opp == 'i'){


					insert(root, val);

				}else if(opp =='s'){

					search(root, val);


				}else{
					printf("error\n");

					return 0;
				}

			}
			fclose(file);


		}
	}else{


		printf("%s\n", "error");
		return 0;
	}







	clearSpace(root);
	return 0;

}

