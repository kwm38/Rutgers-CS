#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

char* compressString(char original[],int len);
char* compressString(char original[],int len)
{

	char * new = malloc(sizeof(char)*( len*2 +1) );

	new[0]=original[0];

	int pointer,count,index;

	index=0;
	int pos=1;
	pointer=index+1;
	while(index < len){

		count =1;

		while(original[index] == original[pointer]){

			count++;
			pointer++;

		}

		new[pos]=count + '0';

		pos++;
		new[pos]= original[pointer];

		pos++;
		index = pointer;
		pointer = index+1;
	}


	return new;
}


int main(int argc, char **argv){
	char *  str, *compressed;
	int sizeO, sizeN;

	if(argv[1] == NULL || argc>2){
		printf("error\n");
		return(0);
	}

	str=argv[1];

	int i=0;
	sizeO = strlen(str);
	while(i < sizeO) {
		if(isdigit(str[i])) {
			printf("error\n");
			return(0);;
		}
		++i;
	}


	compressed = compressString(str,sizeO);


	sizeN= strlen(compressed);

	if(sizeN < sizeO){

		printf("%s\n", compressed);

	}else{

		printf("%s\n", str);

	}

	return 0;

}

