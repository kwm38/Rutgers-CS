#include "indexer.h"

/* a1 */

/* global variable that points to our BST which holds all of our data */
Node * head;
int done;

/* main function that exectures our indexer */
int main(int argc, char** argv){
    
    /* must have 3 arguements -- program writepath readpath -- */
    if(argc != 3){
        printf("An invalid number of  arguements, we need two arguements total (not including the program)\n");
        exit(0);
        
    }else if(strcmp(argv[1],"") == 0 || argv[1] == NULL){
        
        printf("Invalid Output File Name");
        exit(0);
        
    }else if(strcmp(argv[2],"") == 0 || argv[2] == NULL){
        
        printf("Invalid Input File Name");
        exit(0);
    }
    
    /*
     * check to make sure we have permission to write to our output file
     * if it doesn't exist, create it with read write execute permissions
     */
    int outputCheck = open(argv[1], O_RDWR| O_CREAT, 0666);
    
    /* if we have permission to write to the file and it exists , ask if user wants to overwrite it, else if no permission exit */
    if(outputCheck > 0){
        
        char answer = 0;
        char garbage = 0;
        printf("\nA file with the name %s already exists or was just created. Do you want to overwrite or write to it? Enter Y or N\n", argv[1]);
        
        while (answer != 'y' && answer != 'n' && garbage != 10){
            
            scanf("%c", &answer);
            scanf("%c", &garbage);
            fseek(stdin,0,SEEK_END);
            
            if(answer == 'n' && garbage == 10){
                
                printf("We will not overwrite the file. Closing program.\n");
                return 0;
                
            }else if(tolower(answer) == 'y' && garbage == 10){
                
                break;
                
            }else {
                
                printf("Invalid input. Enter Y or N.\n");
                garbage = 0;
                answer = 0;
            }
        }
    }else{
        
        printf("We do not have permission to write to the file %s \n", argv[1]);
        exit(0);
    }
    
    close(outputCheck);
    
    /* we will have a thread keep track of the running time */
    pthread_t timer_thread;
    
    if(pthread_create(&timer_thread, NULL, timer, NULL)) {
        
        fprintf(stderr, "Error creating thread\n");
        return 1;
        
    }
    
    head = NULL;
    
    openSource(argv[2]);
    
    if(head != NULL){
        
        FILE * output = fopen(argv[1], "w");
        fprintf(output,"<\"?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        fprintf(output,"<fileIndex>\n");
        printAndFree(output,head, 1);
        fprintf(output,"<fileIndex>\n");
        
    }else{
        
        if(head == NULL){
            
            printf("We could not find and/or read any files\n");
            exit(0);
        }
    }
    done = 1;
    return 0;
}
/* opens argv[2] and determines whether or not it is a file or directory */
void openSource(char* argv){
    
    /* open the source file */
    int fd = open(argv, O_RDONLY);
    
    if(fd == -1){
        
        printf("%s for the read from file %s\n",strerror(errno), argv);
        return;
    }
    
    int type = 0;
    type = isDirectory(argv);
    /* if its a directory */
    if(type == 1){
        
        close(fd);
        DIR * fdDir = opendir(argv);
        readDir(fdDir);
        return;
    }
    
    type = isFile(argv);
    /* if its a file */
    if(type == 1){
        
        char * name = getName(argv);
       // name = lowerName(name);
        readFile(fd, name);
        return;
    }
    /* neither a file nor directory */
    printf("Your input is not a readable file nor directory\n");
    exit(0);
    
    return;
}

/* helper method to get just the name of the file, if the original input (argv[2]) is a file path */
char * getName(char * argv){
    
    int size =0;
    int length = strlen(argv);
    int i =0;
    
    for(i = length - 1; i >= 0; i--){
        
        if(argv[i] == '/' && i != 0){
            
            break;
        }
        size ++;
    }
    
    char * name = malloc(length + 1);
    int difference = length - size;
    strncpy(name, argv + difference, size);
    name[size] = '\0';
    return name;
}
/* checks to see if the file descriptor is a directory */
int isDirectory( const char *path){
    
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}
/* checks to see if the file descriptor is a file */
int isFile( const char *path){
    
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

/* recursively checks the directory for further directories or files */
void readDir(DIR * fdDir){
    
    if (fdDir){
        struct dirent *dir;
        dir = readdir(fdDir);
        dir = readdir(fdDir);
        
        while ((dir = readdir(fdDir)) != NULL){
            
            /* our dirent structure holds a file */
            if(dir->d_type == DT_REG ){
                
                //  printf("%s\n", dir->d_name);
                int fd = openat(dirfd(fdDir), dir->d_name, O_RDONLY);
                
                if(fd >0){
                    readFile(fd, dir->d_name);
                    
                }else{
                    
                    printf("%s for the read from file %s\n",strerror(errno), dir->d_name);
                    continue;
                }
            }
            /* our dirent structure holds a directory */
            if(dir->d_type == DT_DIR){
                
                int fd = openat(dirfd(fdDir), dir->d_name, O_RDONLY);
                
                if(fd >0){
                    
                    DIR * child = fdopendir(fd);
                    readDir(child);
                }else {
                    
                    printf("%s for the read from file %s\n",strerror(errno), dir->d_name);
                    continue;
                }
            }
        }
    }
    
    closedir(fdDir);
    return;
}

/* takes in a file descriptor as an arguement, and parses the file for string tokens. All tokens are inserted into a BST*/
void readFile(int fd, char * name){
    
    char * word = malloc(sizeof(char) * 2);
    word[1] = '\0';
    int sizeOfWord = 0;
    int pos = 0;
    char * x = malloc(sizeof(char) * 2);
    
    while(read(fd,x,1) != 0){
        
        /* if what we read in is not an alphabetical character or is not 0 - 9 continue */
        if(isalpha(*x) == 0 && !ValidDigit(x)){
            
            continue;
            /* if what we read in is 0 - 9 but our token is empty, continue because tokens cant start with numbers */
        }else if(ValidDigit(x) && strlen(word) < 2){
            
            continue;
        }
        /* if what we read in is alphabetical or 0 - 9 and our token starts with a letter */
        while(isalpha(*x) != 0 || (ValidDigit(x) && strlen(word) >= 1)){
            
            sizeOfWord ++;
            word = realloc(word, sizeOfWord + 1);
            word[pos] = tolower(*x);
            word[pos + 1] = '\0';
            ++pos;
            if( read(fd,x,1) == 0){
                
                break;
            }
        }
        head =  insert(head, word, name, sizeOfWord);
        word = NULL;
        pos = 0;
        sizeOfWord =0;
        word = malloc(sizeof(char) * 2);
        word[1] = '\0';
        
    }
    
    close(fd);
    return;
}
/* checks to make sure the digit in our token is valid */
int ValidDigit(char* x){
    
    switch(*x){
            
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return 1;
    }
    
    return 0;
}

/* inserts a new word into the BST. If the word already exists, increment the count for that word in its respective file */
Node* insert(Node *head, char* word,char* name, int size){
    
    if(head == NULL){
        
        head = (Node*)malloc(sizeof(Node));
        head->word = (char*)malloc(sizeof(char)*(size +1));
        strcpy(head->word,word);
        head->left = head->right = NULL;
        head->fileList = (File*) malloc(sizeof(File));
        head->fileList->name = (char*)malloc(sizeof(char) * strlen(name) + 1);
        strcpy(head->fileList->name,name);
        
        head->fileList->count = 1;
        head->fileList->next = NULL;
        return head;
    }
    
    if(strcmp(head->word, word) == 0 ){
        
        File * temp = head->fileList;
        File * prev = NULL;
        
        while(temp != NULL){
            
            if(strcmp(temp->name, name) == 0){
                
                ++temp->count;
                break;
            }
            prev = temp;
            temp = temp->next;
        }
        
        /* we found no file in the word's file list that matches the file we have open, so lets add this open file to the words file list */
        if(temp == NULL){
            
            temp = (File *) malloc(sizeof(File));
            temp->name = malloc(sizeof(char) * strlen(name) + 1);
            strcpy(temp->name,name);
            temp->count = 1;
            temp->next = NULL;
            prev->next = temp;
        }
    }
    
    if(strcmp(head->word, word) <0 ){
        
        head->left = insert(head->left, word, name, size);
        
    }else  if(strcmp(head->word, word) >0 ){
        
        head->right = insert(head->right, word, name, size);
    }
    return head;
}

/*
// converts the files name to lowercase
char * lowerName(char * name){
    
    char * lowerName = malloc(sizeof(char)* strlen(name) + 1);
    
    int i=0;
    for(i=0; i< strlen(name) + 1; i ++){
        
        lowerName[i] = tolower(name[i]);
        
    }
    
    return lowerName;
}

*/

/* prints the content of the BST then frees the node and its file list*/
void printAndFree(FILE * output, Node * head, int permission){
    
    if(head->right != NULL){
        
        printAndFree(output,head->right,permission);
    }
    
    if(permission){
        
        fprintf(output, "\t<word text=\"%s\">\n", head->word);
        
        head->fileList = sortByCount(head->fileList);
        
        while(head->fileList != NULL){
            
            fprintf(output,"\t\t<file name=\"%s\">%d</file>\n", head->fileList->name, head->fileList->count);
            
            File * prev = head->fileList;
            head->fileList = head->fileList->next;
            free(prev);
        }
    }
    
    if(head->left != NULL){
        
        printAndFree(output,head->left,permission);
    }
    
    free(head);
}
/* sorts the files associated with a word corresponding to their count in descending order */
File * sortByCount(File * list){
    
    if(list == NULL || list->next == NULL){
        
        return list;
    }
    
    int swapped;
    File * file1;
    File * lptr = NULL;
    
    do{
        swapped = 0;
        file1 = list;
        
        while (file1->next != lptr) {
            
            if (file1->count < file1->next->count){
                
                int temp = file1->count;
                char * tempString = malloc(sizeof(char) * strlen(file1->name) + 1);
                strcpy(tempString, file1->name);
                free(file1->name);
                file1->name = malloc(strlen(file1->next->name) + 1);
                strcpy(file1->name, file1->next->name);
                free(file1->next->name);
                file1->next->name =  malloc(strlen(tempString) + 1);
                strcpy(file1->next->name, tempString);
                file1->count = file1->next->count;
                file1->next->count = temp;
                swapped = 1;
            }
            file1 = file1->next;
        }
        lptr = file1;
    }
    while (swapped);
    
    return list;
}

/* this timer will be excuted by a thread. This will always update the running time and print it to the consol */
void * timer (){
    
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    double elapsed = 0;
    
    while (!done){
        
        gettimeofday(&end, NULL);
        elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
        printf("%lf seconds\r", elapsed);
        
    }
    printf("%lf seconds\n", elapsed);
    printf("Complete\n");
    return &done;
}
