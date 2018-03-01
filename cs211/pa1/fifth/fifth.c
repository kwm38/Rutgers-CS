#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char ** argv){

	FILE *file;

	int n,i,j,val,x;

	file = fopen(argv[1], "r");

	if(file ==NULL){

		printf("error\n");
		return 0;

	}
	char * power = malloc(sizeof(int));
	fscanf(file,"%s",power);
	n =(int) atoi(&power[0]);

	int ** matrix;
	matrix = (int**) malloc(n* sizeof(int*));

	for(i =0; i<n; i++){

		matrix[i] = (int*) malloc(n * sizeof(int));
	}

	for(i =0; i<n; i++){

		for(j=0; j<n; j++){
			fscanf(file,"%s",power);

			matrix[i][j]=(int) atoi(power);
		}

	}


	val =0;
	for(i =0; i<n; i++){
		val=0;
		for(j=0; j<n; j++){


			val += matrix[i][j];

		}


		if(val != 15){

			printf("not-magic\n");

			for ( x=0; x<n; x++){

				free(matrix[x]);

			}
			free(matrix);
			return 0;

		}
	}



	val =0;
	for(i =0; i<n; i++){

		val =0;

		for(j=0; j<n; j++){


			val += matrix[j][i];

		}


		if(val != 15){

			printf("not-magic\n");

			for ( x=0; x<n; x++){

				free(matrix[x]);

			}
			free(matrix);
			return 0;

		}
	}


	val =0;
	for(i =0; i<n; i++){





		val += matrix[i][i];





	}
	if(val != 15){

		printf("not-magic\n");

		for ( x=0; x<n; x++){

			free(matrix[x]);

		}
		free(matrix);
		return 0;

	}
	val =0;
	for(i =0; i<n; i++){



		val += matrix[i][n-i-1];




	}

	if(val != 15){

		printf("not-magic\n");

		for ( x=0; x<n; x++){

			free(matrix[x]);

		}
		free(matrix);
		return 0;

	}

	printf("magic\n");

	for ( x=0; x<n; x++){

		free(matrix[x]);

	}
	free(matrix);
	return 0;
}






