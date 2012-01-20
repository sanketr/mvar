#include "mvar.h"
#include <stdio.h>
#include <pthread.h>

void *threadFunc(void *var)
{
  int i;
  int* a = (int*) malloc (sizeof(int));
  *a = 0;
  for (i=0; i < 10000000;i++){
     *a=i;
     putMVar((mvar*)var,a);
     a = takeMVar((mvar*)var);
  }
 free(a);
 return NULL;
}




int main(int argc, char *argv[]){
  int t;
  int numt=2;
  pthread_t pth[numt];

  if(argc < 2){
    printf("Need number of threads as first argument\n");
    return 0;
  }
  numt = atoi(argv[1]);
  mvar* mvar1[numt];

  for (t=0; t < numt;t++){
    mvar1[t] = newEmptyMVar();
    pthread_create(&pth[t],NULL,threadFunc,mvar1[t]);
  }

  for (t=0; t < numt;t++)
    pthread_join(pth[t],NULL);

  return 0;
}
