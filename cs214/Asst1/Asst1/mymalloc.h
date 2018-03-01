#ifndef mymalloc_h
#define mymalloc_h

#define malloc( x ) mymalloc( (x) , __FILE__ , __LINE__ )
#define free( x ) myfree( (x) , __FILE__ , __LINE__ )

/* this function will attempt to malloc memory in our pseudo memory which is of size 5000 */
void *mymalloc(unsigned int size, char *file, int line);
/* this function will attempt to free memory in our pseudo memory which is of size 5000 */
void myfree(void *p, char *file, int line);

#endif

