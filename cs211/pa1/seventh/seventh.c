#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char **argv){


int i=0;

for (i=1; i<argc;i++){
char*  str = argv[i];
int length = strlen(str);
printf("%c", argv[i][length-1]);



}
printf("\n");

return 0;

}
