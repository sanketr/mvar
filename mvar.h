#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct{
  pthread_cond_t put_cond;
  pthread_cond_t take_cond;
  pthread_mutex_t lock;
  void* value;
} mvar;

mvar* newMVar(void*);
mvar* newEmptyMVar();
void* readMVar(mvar*);
void  putMVar(mvar*, void*);
void* takeMVar(mvar*);
void  deleteMVar(mvar*);
