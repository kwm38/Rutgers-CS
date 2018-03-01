#include <stdio.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    long tag;
    int valid;
    long order;
    long timestamp;
    
} Line;
void initialize();
void readfile();
int opperate(long tag, int index, Line ** cache, int pf);
void clearMem();
