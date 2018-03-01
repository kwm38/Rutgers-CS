#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


int main(int argc, char **argv){
       
FILE *file;
char * str = malloc(sizeof(char*));
file = fopen(argv[1], "r");
fscanf(file, "%s", str);
int total = atoi(&str[0]);	


int list[total];
int count=0;

    while(!feof(file)){
	    if(count == total){
	    break;
}

     fscanf(file, "%s", str); 
     int num = atoi(str);
     list[count] = num;
     
	
     count++;
}


int temp,i,j;

   for (i = 0; i < total; i++)
    {
        for (j = 0; j < (total - i - 1); j++)
        {
            if (list[j] > list[j + 1])
            {
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }

    }




for(i=0; i<total; i++){

printf("%d\t", list[i]);

}

printf("\n");
             
return 0;
    
}

