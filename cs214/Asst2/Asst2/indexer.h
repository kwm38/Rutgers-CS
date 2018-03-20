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
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

/* This struct will be the way we keep track of files that contain a specific word  */
struct File
{
    char * name;
    int count;
    struct File *next;
    
}typedef File;

/* This struct will be the building blocks for our binary tree.  */
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

/* printAndFree traverses the BST printing out the XML format. It prints the word at the node it is currently on, then prints the list of files that contain that word in descending order relative to the frequency of the given word in that file
 */
void printAndFree(FILE * output, Node * head, int permission);
/*
 * lowerName takes the input token (filename or string token) and converts it to lowercase
 */
char * lowerName(char * name);
/*
 * sortByCount takes the File list of a node and sorts it in descending order according to its count
 * count is the frequency of whitch a given word appeared in the file
 */
File * sortByCount(File * list);
/*
 * openSource takes the input in arguement 2 and opens it. It will determine whether or not the input is a file or directory
 */
void openSource(char* argv);
/*
 * readDir reads the given directory and will call the readFile function or recursively open another directory
 */
void readDir(DIR * fdDir);
/*
 * readFile reads the given given file for tokens, inserting each token into the BST
 */
void readFile(int fd, char * name);
/*
 * isDirectory determines whether or not our file descriptor is directory
 */
int isDirectory( const char *path);
/*
 * isFile determines whether or not our file descriptor is file
 */
int isFile( const char *path);
/*
 * getName will get the name of the file if arguement two is a path to a single file
 */
char * getName(char * argv);
/*
 * lowerName will take the filename and turn it to lowercase
 */
char * lowerName(char * name);
/*
 * ValidDigit will check to see if a potential token character is between 0 - 9
 */
int ValidDigit(char* x);
/*
 * timer will be executed by a thead in order to keep track of and display the running time during the entirety of the program.
 */
void * timer ();
#endif
