#ifndef indexer_h
#define indexer_h

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

/*
 * This struct will be the way we keep track of files that contain a specific word
 */
struct File
{
    char * name;
    int count;
    struct File *next;
    
}typedef File;
/*
 * This struct will be the building blocks for our binary tree.
 */
struct Node
{
    char * word;
    struct Node *left;
    struct Node *right;
    File * fileList;

}typedef Node;
/*
 * insert is a function that will traverse the tree, comparing each nodes string with the new string ready for insert.
 * The new string will be placed in its sorted order, where a new node will be made to hold the string.
 */
Node* insert(Node *head, char* temp,char* name, int size);
/*
 * printAndFree traverses the string, printing all words starting with capital letters first, then lowercase.
 * ex: (A->Z->a->z)
 * Once a node has had all of the strings less than its string printed, it will print its node, then print the strings greater then it.
 * Once a node is no longer needed, its memory will be freed.
 */
char * lowerName(char * name);
File * sortByCount(File * list);
void printAndFree(FILE * output, Node * head, int permission);
void openSource(char* argv);
void readDir(DIR * fdDir);
void readFile(int fd, char * name);
int isDirectory( const char *path);
int isFile( const char *path);
char * getName(char * argv);
#endif
