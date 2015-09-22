#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
pthread_mutex_t lock;
typedef struct dict {
  char *word;
  int count;
  struct dict *next;
} dict_t;
 
dict_t *diction;
FILE *infiles;
char * bufs;
char * make_word( char *word ) {
  return strcpy( malloc( strlen( word )+1 ), word );
}

dict_t * make_dict(char *word) {
  dict_t *nd = (dict_t *) malloc( sizeof(dict_t) );
  nd->word = make_word( word );
  nd->count = 1;
  nd->next = NULL;
  return nd;
}

dict_t *
insert_word( dict_t *d, char *word ) {
  
  //   Insert word into dict or increment count if already there
  //   return pointer to the updated dict
  pthread_mutex_lock(&lock);
  dict_t *nd;
  dict_t *pd = NULL;		// prior to insertion point 
  dict_t *di = d;		// following insertion point
  // Search down list to find if present or point of insertion
  while(di && ( strcmp(word, di->word ) >= 0) ) { 
    if( strcmp( word, di->word ) == 0 ) { 
      di->count++;		// increment count 
      pthread_mutex_unlock(&lock);
      return d;			// return head 
    }
    pd = di;			// advance ptr pair
    di = di->next;
  }
  nd = make_dict(word);		// not found, make entry 
  nd->next = di;		// entry bigger than word or tail 
  if (pd) {
    pd->next = nd;
    pthread_mutex_unlock(&lock);
    return d;			// insert beond head 
  }
  pthread_mutex_unlock(&lock);
  return nd;
}
void print_dict(dict_t *d) {
  while (d) {
    printf("[%d] %s\n", d->count, d->word);
    d = d->next;
  }
}

int get_word( char *buf, int n, FILE *infile) {
  pthread_mutex_lock(&lock);
  int inword = 0;
  int c;  
  //pthread_mutex_t lock;
  
  while( (c = fgetc(infile)) != EOF ) {
    if (inword && !isalpha(c)) {
      buf[inword] = '\0';	// terminate the word string
      pthread_mutex_unlock(&lock);
      return 1;
    } 
    if (isalpha(c)) {
      buf[inword++] = c;
    }
  }
    pthread_mutex_unlock(&lock);
  return 0;			// no more words
}


#define MAXWORD 1024
void * Words(void *args){
  //dict_t *wd = NULL;
  char wordbuf[MAXWORD];
  int check = get_word( wordbuf, MAXWORD, infiles );
  while( check ) {
    diction = insert_word(diction, wordbuf); // add to dict
    check = get_word( wordbuf, MAXWORD, infiles );
    if(check == 0)
    return;
  }
  //diction;
 
}
int main(int argc, char *argv[]){
 diction = NULL;
 infiles = stdin;
 bufs = (char *)malloc(MAXWORD);
 if(argc >= 2){
  infiles = fopen(argv[1],"r");
 }
 if( !infiles ) {
    printf("Unable to open %s\n",argv[1]);
    exit( EXIT_FAILURE );
  }
 pthread_t thread[4];
 int i =0;
 for(i =0; i < 4; i++)
 pthread_create(&thread[i], NULL, Words, NULL);
 for(i =0; i < 4; i++)
 pthread_join(thread[i],NULL);
 print_dict( diction );
  fclose( infiles );
}
