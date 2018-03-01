#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>


double ** matrix;
double *y;
int width;
int height;
int j,k,r,i,c;
double total,val,num;

double** transpose(){
//	printf("\n");
	double **  matrixT = (double**) malloc( width *sizeof(double*));
//	printf("transposed matrix\n");

	for(j =0; j<width; j++){
		matrixT[j] = (double*) malloc((height) * sizeof(double));
	}


	for(j =0; j<width; j++){

		for(i=0; i< height; i++){

			matrixT[j][i] = matrix[i][j];
//			printf("%lf ", matrixT[j][i]);

		}
//		printf("\n");

	}

	return matrixT;


}

double ** multiply(double**matrixT,double ** matrix){
//	printf("\n");
//	printf("multiplied matrix\n");
	double ** matrixTO;

	matrixTO = (double**) malloc( width *sizeof(double*));
	for(j =0; j<width; j++){
		matrixTO[j] = (double*) malloc((width) * sizeof(double));
	}

	for(c=0;c<width;c++){
		for (i=0;i<width;i++){
			total =0.0f;
			for(j=0;j<height;j++){

				total += matrixT[c][j] * matrix[j][i];



			}

			matrixTO[c][i]= total;
//			printf("%lf ", matrixTO[c][i]);
		}

//		printf("\n");

	}
	return matrixTO;

}

double ** augmentIdentity(double ** matrixTO){
//	printf("\n");
//	printf("augmented matrix\n");
	double ** aug = (double**) malloc(width * sizeof(double*));

	for(j=0;j<width;j++){

		aug[j] = (double *) malloc((width*2) * sizeof(double));

	}

	for (i=0; i<width; i++){

		for(j=0; j<width; j++){

			aug[i][j]=matrixTO[i][j];
//			printf("%lf ", aug[i][j]);
		}


		for(c=width; c<width *2; c++){

			if(c==(width+i)){


				aug[i][c]=1.0f;
			}else{

				aug[i][c]=0.0f;
			}
//			printf("%lf ", aug[i][c]);
		}
//		printf("\n");
	}

	return aug;
}

double ** reduction(double ** aug, int count){
//	printf("\n");


	while(count != width){

		for(i=count;i<width;i++){

			double val=aug[i][count];
			for(j=0;j<width*2;j++){
				aug[i][j]= aug[i][j]/val;
			}
		}

		double* top=aug[count];

		for(r=1;r<width;r++){
			if(r==count){continue;}
			double* bottom =aug[r];

			for(k=0;k<width*2;k++){
				bottom[k] -= top[k];
			}




		}

		count++;
	}


/*
	printf("lower trianfgular \n");
	for(j=0;j<width;j++){
		for(c=0;c<width*2;c++){

			printf("%lf ", aug[j][c]);
		}               printf("\n");


	}

*/
	return aug;

}
double ** reverseReduction(double ** aug,int count){

	while(count >= 0){
		for(i=count;i>=0; i--){

			val=aug[i][count];
			for(j=width*2-1;j>=0;j--){

				aug[i][j]= aug[i][j]/val;

			}
		}


		double* top=aug[count];

		for(r=count-1;r>=0;r--){
			if(r==count){continue;}
			double* bottom =aug[r];

			for(k=width*2-1; k>=0;k--){

				bottom[k] -= top[k];
				if(bottom[k]== - 0.0){bottom[k]=0.0;}

			}

		}

		count --;
	}



	return aug;
}
double ** invert(double ** reduc){
/*	printf("Reduction matrix\n");
	for(j=0;j<width;j++){
		for(c=0;c<width*2;c++){

			printf("%lf ", reduc[j][c]);
		}               printf("\n");


	}
*/




	double ** inverse = (double **) malloc( width * sizeof(double*));


//	printf("inverse matrix\n");
	for(i=0;i<width;i++){

		inverse[i]= (double*) malloc(width *(sizeof(double)));

	}

	for(i=0;i<width;i++){
		int count=0;
		for(j=0;j<width;j++){
			inverse[i][j]=reduc[i][width+count];
//			printf("%lf ",inverse[i][j]);
			count ++;
		}



//		printf("\n");
	}

	return inverse;

}

double ** pseudoInverse(double**inverse,double ** matrixT){
//	printf("\n");

//	printf("pseudo inverse matrix\n");

	double ** pseudo = (double **) malloc( width * sizeof(double*));
	for(i=0;i<width;i++){

		pseudo[i]= (double*) malloc(height *(sizeof(double)));

	}


	for(c=0;c<width;c++){
		for (i=0;i<height;i++){
			num =0.0f;
			for(j=0;j<width;j++){

				num += inverse[c][j] * matrixT[j][i];



			}

			pseudo[c][i]= num;
//			printf("%lf ", pseudo[c][i]);
		}

//		printf("\n");

	}




	return pseudo;

}


double * getVector(double ** pseudo, double *y){
//	printf("\nw vector\n");
	double* w = malloc(width * sizeof(double));

	for(i=0; i<width; i++){
		val =0.0f;
		for(j=0;j<height; j++){

			val += pseudo[i][j] * y[j];


		}

		w[i] = val;

//		printf("%lf ", w[i]);
	}

//	printf("\n\n");
	return w;
}

void output (double ** newMatrix, double * w){
//	printf("\nresults\n");

	for(i=0; i<height; i++){
		val=0;
		for(j=0;j<width; j++){

			val += newMatrix[i][j] * w[j];



		}

		printf("%0.0lf\n", val);


	}
	return;


}
void clear (double ** matrix, int rows){

	for(i=0;i<rows; i++){
		free(matrix[i]);
	}
	free(matrix);

}


int main(int argc, char** argv){

	FILE *file = fopen(argv[1], "r");
	if(file == NULL){

		printf("error\n");

		return 0;

	}


	fscanf(file,"%d",&width);
	width++;
	//printf("%d\n", width);
	fscanf(file,"%d",&height);
	//printf("%d\n", height);

	matrix = (double **) malloc(height*sizeof(double*));
	y = malloc(height * sizeof(double));
	for(i =0; i<height; i++){
		matrix[i] = (double*) malloc((width ) * sizeof(double));
		matrix[i][0]=1.0f;
	}


	for(i=0;i<height;i++){


		for(j=1;j<=width; j++){

			if(j== width){
				fscanf(file,"%lf", &y[i]);
				continue;

			}
			fscanf(file,"%lf", &matrix[i][j]);
			fscanf(file,",");			

		}
	}


	fclose(file);
	///print original and y
/*
	printf("original matrix\n");
	for(i=0;i<height;i++){
		for(j=0;j<width;j++){
			printf("%lf ",matrix[i][j]);}
		printf("\n"); }
	printf("y vector\n");

	for(i=0;i<height;i++){

		printf("%lf\n",y[i]);

	}
*/
	double ** matrixT= transpose();
	double ** matrixTO = multiply(matrixT, matrix);
	double ** aug = augmentIdentity(matrixTO);
	aug= reduction(aug, 0);
	double** reduc = reverseReduction(aug, width-1);
	double ** inverse = invert(reduc);
	double ** pInverse= pseudoInverse(inverse, matrixT);
	double *w = getVector(pInverse, y);


	clear(matrix,height);
	clear(matrixT,width);
	clear(matrixTO,width);
	clear(aug,width);
	clear(inverse,width);
	clear(pInverse,width);




	file = fopen(argv[2], "r");
	if(file == NULL){

		printf("error\n");

		return 0;

	}


	fscanf(file,"%d",&height);
//	printf("%d\n", height);


	double ** test = (double**) malloc( height *sizeof(double*));
	for(i =0; i<height; i++){
		test[i] = (double*) malloc((width) * sizeof(double));
		test[i][0] =1.0f;
	}

	for(i=0;i<height;i++){


		for(j=1;j<width; j++){

			fscanf(file,"%lf", &test[i][j]);
			fscanf(file,",");

		}
	}	
/*
	printf("new matrix\n");
	for(i=0;i<height;i++){
		for(j=0;j<width;j++){
			printf("%lf ",test[i][j]);}
		printf("\n"); }
*/
	fclose(file);

	output(test, w);

	clear(test,height);
	free(w);
	free(y);


	return 0;
}
