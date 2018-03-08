#include "indexer.h"

Node * head;
/* main function that exectures our indexer */
int main(int argc, char** argv){
    
    if(argc != 3){
        printf("An invalid number of  arguements, we need two arguements total (not including the program)\n");
        exit(0);
        
    }else if(strcmp(argv[1],"") ==0 || argv[1] == NULL){
        
        printf("Invalid Output File Name");
        exit(0);
        
    }else if(strcmp(argv[2],"") ==0 || argv[2] == NULL){
        
        printf("Invalid Input File Name");
        exit(0);
    }
    
    int outputCheck = open(argv[1], O_RDWR|O_CREAT, 0777);
    
    if(outputCheck == -1){
        
        printf("%s for the write to file\n",strerror(errno));
        exit(0);
    }
    
    close(outputCheck);
    
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
        
        printf("We do not have permission to write to the file\n");
        exit(0);
    }
    return 0;
}
/* opens argv[2] and determines whether or not it is a file or directory */
void openSource(char* argv){
    
    
    int fd = open(argv, O_RDONLY);
    
    if(fd == -1){
        
        printf("%s for the read from file\n",strerror(errno));
        return;
    }
    
    int type = 0;
    type = isDirectory(argv);
    
    if(type == 1){
        
        close(fd);
        DIR * fdDir = opendir(argv);
        readDir(fdDir);
        return;
    }
    
    type = isFile(argv);
    if(type == 1){
        
        char * name = getName(argv);
        name = lowerName(name);
        readFile(fd, name);
        return;
    }
    
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
            // printf("%s\n", dir->d_name);
            if(dir->d_type == DT_REG ){//}&& strcmp(dir->d_name, ".git") != 0){
                
                //  printf("%s\n", dir->d_name);
                int fd = openat(dirfd(fdDir), dir->d_name, O_RDONLY);
                readFile(fd, lowerName(dir->d_name));
                
            }
            if(dir->d_type == DT_DIR && strcmp(dir->d_name, ".git") != 0 && strcmp(dir->d_name, ".metadata") != 0){
                
                //  printf("%s\n", dir->d_name);
                int fd = openat(dirfd(fdDir), dir->d_name, O_RDONLY);
                DIR * child = fdopendir(fd);
                readDir(child);
            }
        }
    }
    
    closedir(fdDir);
    
    return;
}

/* takes in a file descriptor as an arguement, and parses the file for string tokens. All tokens are inserted into a BST*/
void readFile(int fd, char * name){
    
    
    char * word = malloc(sizeof(char) * 2);
    int sizeOfWord = 0;
    int pos = 0;
    char * x = malloc(sizeof(char) * 2);
    
    while(read(fd,x,1) != 0){
        
        if(isalpha(*x) == 0){
            continue;
        }
        
        while(isalpha(*x) != 0){
            
            sizeOfWord ++;
            word = realloc(word, sizeOfWord + 1);
            word[pos] = tolower(*x);
            word[pos + 1] = '\0';
            ++pos;
            //printf("%c",*x);
            if( read(fd,x,1) == 0){
                
                break;
            }
        }
        //printf("\n");
        head =  insert(head, word, name, sizeOfWord);
        free(word);
        pos = 0;
        sizeOfWord =0;
        word = malloc(sizeof(char) * 1);
        
    }
    
    close(fd);
    return;
}

/* inserts a new word into the best. If the word already exists, increment the count for that word in its respective file */
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
        // printf("worde %s", head->word);
        
        File * temp = head->fileList;
        File * prev = NULL;
        // int inserted = 0;
        
        while(temp != NULL){
            
            if(strcmp(temp->name, name) == 0){
                
                ++temp->count;
                // inserted = 1;
                break;
            }
            prev = temp;
            temp = temp->next;
        }
        
        if(temp == NULL){
            // printf("neq name %s\n", name);
            // printf("neq name %s\n", temp->name);
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


/* converts the files name to lowercase */
char * lowerName(char * name){
    
    char * lowerName = malloc(sizeof(char)* strlen(name) + 1);
    
    int i=0;
    for(i=0; i< strlen(name) + 1; i ++){
        
        lowerName[i] = tolower(name[i]);
        
    }
    
    return lowerName;
    
}

/* prints the content of the BST */
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
/* sorts the files associated with a word corresponding to their size in descending order */
File * sortByCount(File * list){
    
    if(list == NULL || list->next == NULL){
        
        return list;
    }
    
    int swapped, i;
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
