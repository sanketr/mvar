#include <stdio.h>
#include <pthread.h>

void* threadFunc(void *var)
{
  int i;
  for(i=0; i < 10000000;i++){
    pthread_mutex_lock(var);
    pthread_mutex_unlock(var);
  }
}

typedef struct data{
  int numt;
  pthread_mutex_t* lock;
} data;

void* testing(void* arg){
  struct timespec req;
  struct timespec rem;
  data* tdata = (data*) arg;
  int numt = tdata->numt;
  pthread_mutex_t* lock = tdata->lock;
  req.tv_sec = 0;
  req.tv_nsec = 25000;
  nanosleep(&req,&rem);
  int i;
  int j;
  for(i=0; i < 20;i++){
      pthread_mutex_lock((tdata->lock) + 0);
      pthread_mutex_unlock((tdata->lock) + 0);
  }

}

int main(int argc, char *argv[]){
  int t;
  int numt;

  if(argc < 2){
    printf("Need number of threads as first argument\n");
    return 0;
  }
  numt = atoi(argv[1]);
  pthread_mutex_t lock[numt];
  pthread_t pth[numt+1];
  data tdata;
  tdata.numt = numt;
  tdata.lock = lock;

  for (t=0; t < numt;t++){
    printf("Creating thread %i\n",t);
    pthread_mutex_init(&lock[t],NULL);
    pthread_create(&pth[t],NULL,threadFunc,&lock[t]);
  }

  pthread_create(&pth[t],NULL,testing,&tdata);

  for (t=0; t < numt+1;t++)
    pthread_join(pth[t],NULL);

  return 0;
}
