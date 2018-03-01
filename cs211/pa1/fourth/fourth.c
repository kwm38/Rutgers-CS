#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char ** argv){

	FILE *file;
	int n,i,j,r,x;

	file = fopen(argv[1], "r");

	if(file ==NULL){

		printf("error\n");
		return 0;

	}

	char* power = malloc(sizeof(char)*20);
	fscanf(file,"%s",power);
	n =(int) atoi(&power[0]);

	int ** matrix;
	int **final;
	int **answer;
	matrix = (int**) malloc(n* sizeof(int*));
	final = (int**) malloc(n*sizeof(int*));
	answer= (int**) malloc(n*sizeof(int*));
	for(i =0; i<n; i++){

		matrix[i] = (int*) malloc(n * sizeof(int));
		final[i] = (int*) malloc(n*sizeof(int));
		answer[i]=(int*) malloc(n*sizeof(int));
	}

	for(i =0; i<n; i++){

		for(j=0; j<n; j++){
			fscanf(file,"%s",power);

			matrix[i][j]=(int) atoi(power);
			final[i][j]=(int)atoi(power);
			answer[i][j]=0;
		}
	}


	char * times = malloc(sizeof(char)*20);

	fscanf(file, "%s", times);

	int runs = (int) atoi(times);
	int c,d,e;
	if(runs !=1 && runs!=0){

		for(c=0; c<runs-1; c++){
			for(i =0; i<n; i++){
				for(j=0; j<n; j++){
					int val=0;	
					for(r=0; r<n; r++){
						val+=( final[i][r] * matrix[r][j]);

					}

					answer[i][j] = val;

				}



			}

			for(d=0; d<n; d++){
				for(e=0; e<n; e++){
					final[d][e] = answer[d][e];

				}



			}




		}

	}
	if(runs == 1){
		for(i=0; i< n; i++){

			for(j=0;j<n; j++){

				printf("%d\t",matrix[i][j] );
			}

			printf("\n");

		}

	}else if(runs ==0){

		for(i=0; i< n; i++){

			for(j=0;j<n; j++){
				answer[i][j] =0;
				printf("%d\t",answer[i][j] );
			}

			printf("\n");

		}





	}else{



		for(i=0; i< n; i++){

			for(j=0;j<n; j++){

				printf("%d\t",answer[i][j] );
			}

			printf("\n");

		}
	}

	for ( x=0; x<n; x++){

		free(matrix[x]);
		free(final[x]);
		free(answer[x]);
	}
	free(matrix);
	free(final);
	free(answer);

	return 0;
}
