#include "libnetfiles.h"

int fileMode;
char * hostIP;
int initiated;
char * ret1 = "-1";

/* ---------------- DONE ---------------- */
int netserverinit(char * hostname, int filemode){
    
    if(filemode < 0 || filemode > 2){
        
        errno = INVALID_FILE_MODE;
        return -1;
    }
    /* check to see if a host with this name exists */
    struct hostent * host;
    host = gethostbyname(hostname);
    
    if(host == NULL){
        
        h_errno = errno = HOST_NOT_FOUND;
        return -1;
    }
    /* get the IP address of the host */
    if(inet_ntoa( *( struct in_addr*)( host -> h_addr_list[0])) != NULL) {
        
        hostIP = malloc(sizeof(char) * strlen(inet_ntoa( *( struct in_addr*)( host -> h_addr_list[0]))) + 1);
        strcpy(hostIP,inet_ntoa( *( struct in_addr*)( host -> h_addr_list[0])));
        
    }else{
        
         h_errno = errno = HOST_NOT_FOUND;
        return -1;
    }
    
    fileMode = filemode;
    initiated = 1;
    errno = 0;
    return 0;
}

/* ---------------- DONE ---------------- */
int netopen(const char *pathname, int flags){
    
    if(!initiated){
        
        errno = HOST_NOT_FOUND;
        return -1;
    }
    
    if(flags < 0 || flags > 2){
        
        errno = EACCES;
        return -1;
    }
    
    char * message = (char *) malloc(sizeof(char) * strlen(pathname) + 7);
    char open = 'o';
    char flag = flags + '0';
    /* o:flags:fileMode:path*/
    message[0] = open;
    message[1] =  message[3] = message[5] = ':';
    message[2] = flag;
    message[4] = fileMode + '0';
    strcat(message,pathname);
    
    char * returnMessage = connectServer(message);
    
    int returnValue = atoi(returnMessage);
    
    if(returnValue < 0){
        
        errno = -returnValue;
        returnValue = -1;
        
    }else{
        errno = 0;
        returnValue = -(returnValue);
    }
    return returnValue;
    
}
/* ---------------- DONE ---------------- */
ssize_t netread(int fildes, void *buf, size_t nbyte){
    
    if(!initiated){
        
        errno = HOST_NOT_FOUND;
        return -1;
    }
    if(fildes >= -1){
        
        errno = EBADF;
        return -1;
    }
    if(nbyte < 0 || nbyte > 999999998){
        
        errno = ENOMEM;
        return -1;
    }
    char * message = (char *) malloc(sizeof(char) * 35);
    char read = 'r';
    /* r:size:fd:fileMode:message*/
    message[0] = read;
    message[1] = ':';
    char nbyteChar [15];
    snprintf(nbyteChar, 15, "%zu", nbyte);
    strcat(message , nbyteChar);
    strcat(message,":");
    snprintf(nbyteChar, 15, "%d", (-fildes));
    strcat(message , nbyteChar);
    strcat(message,":");
    snprintf(nbyteChar, 15, "%d", (fileMode));
    strcat(message , nbyteChar);
    strcat(message,":");
    strcat(message , nbyteChar);
    
    char * returnMessage = connectServer(message);
 
    int returnValue = atoi(returnMessage);
    
    if(returnValue == 0){
        
        return 0;
    }
    
    if(returnValue < 0){
        
        errno = -returnValue;
        returnValue = -1;
        
    }else{
        
        returnValue = returnValue;
        
        int i = 0;
        while(returnMessage[i] != ':'){
            i++;
        }
        strcpy((char*)buf , returnMessage + i + 1);
        errno = 0;
    }
    
    
    return returnValue;
}
ssize_t netwrite(int fildes, const void *buf, size_t nbyte){
    
    if(!initiated){
        
        errno = HOST_NOT_FOUND;
        return -1;
    }
    if(fildes >= -1){
        
        errno = EBADF;
        return -1;
    }
    if(nbyte < 0 || nbyte > 999999998){
        
        errno = ENOMEM;
        return -1;
    }
    char * message = (char *) malloc(sizeof(char) * 35 + strlen((char*)buf));
    char write = 'w';
    /* w:size:fd:message */
    message[0] = write;
    message[1] = ':';
    char nbyteChar [15];
    snprintf(nbyteChar, 15, "%zu", nbyte);
    strcat(message , nbyteChar);
    strcat(message,":");
    snprintf(nbyteChar, 15, "%d", (-fildes));
    strcat(message , nbyteChar);
    strcat(message,":");
    strcat(message,buf);
    
    char * returnMessage = connectServer(message);
    
    int returnValue = atoi(returnMessage);
    
    if(returnValue == 0){
        
        return 0;
    }
    
    if(returnValue < 0){
        
        errno = -returnValue;
        returnValue = -1;
        
    }else{
        
        returnValue = returnValue;
        errno = 0;
        
    }
    
    return returnValue;
}

/* ---------------- DONE ---------------- */
int netclose(int fd){
    
    if(!initiated){
        
        errno = HOST_NOT_FOUND;
        return -1;
    }
    
    if(fd >= -1){
        
        errno = EBADF;
        return -1;
    }
    
    char * message = (char *) malloc(sizeof(char) *  21);
    char close = 'c';
    /* c:fileMode:fd*/
    message[0] = close;
    message[1] = message[3] =':';
    message[2] = fileMode + '0';
    snprintf(message + 4, 15, "%d", (-fd));
    
    char * returnMessage = connectServer(message);
    int returnValue = atoi(returnMessage);
    
    if(returnValue < 0){
        
        errno = -returnValue;
        returnValue = -1;
        
    }else{
        
        returnValue = -(returnValue);
        errno = 0;
    }
    return returnValue;
    
}
/* ---------------- DONE ---------------- */
char * connectServer(char * message){
    
    int sock = -1;
    struct sockaddr_in serv_addr;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("%s\n", strerror(errno));
        return ret1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(52678);
    
    if(inet_pton(AF_INET, hostIP, &serv_addr.sin_addr)<=0)
    {
        printf("%s\n", strerror(errno));
        return ret1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("%s\n", strerror(errno));
        return ret1;
    }
    
    int sent = send(sock , message , strlen(message), 0);
    
    if(sent < 0){
        
        printf("%s\n", strerror(errno));
        return ret1;
    }
    
    char * buffer = (char *) malloc(sizeof(char) * strlen(message));
    
    if(message[0] == 'r'){
        
        ssize_t bytes = 0;
        sscanf(message + 2, "%zu", &bytes);
        buffer = (char *) realloc(buffer,bytes + 16);
    }
    
    read(sock , buffer, malloc_usable_size(buffer));
   
    return buffer;
}

