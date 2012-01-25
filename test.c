#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define LOOPS 1000000


typedef struct data{
  int numt;
  pthread_mutex_t* lock;
} data;

/** this is timer thread that simulates binary mutex contention with rest of the threads every req nanoseconds **/
void* timer(void* arg){
  struct timespec req;
  struct timespec rem;
  data* tdata = (data*) arg;
  int numt = tdata->numt;
  pthread_mutex_t* lock = tdata->lock;
  req.tv_sec = 0;
  req.tv_nsec = 25000;
  int i;
  int j;
  for(i=0; i < 1000;i++){
    for(j=0;j < numt; j++){
      pthread_mutex_lock((tdata->lock) + j);
      pthread_mutex_unlock((tdata->lock) + j);
    }
      nanosleep(&req,&rem);
  }
}

/** thread function that tests mutex - each thread has its own copy of mutex - timer function contends for that mutex **/
void* threadFunc(void *var)
{
  int i;
  for(i=0; i < LOOPS;i++){
    pthread_mutex_lock(var);
    pthread_mutex_unlock(var);
  }
}

long diff(struct timespec start,struct timespec end){
  struct timespec temp;
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return ((temp.tv_sec * 1000000000) + temp.tv_nsec);
}

int main(int argc, char *argv[]){
  int t;
  int numt;
  int numi;

  if(argc < 3){
    printf("Need number of threads and number of iterations as first and second arguments\n");
    return 0;
  }
  numt = atoi(argv[1]);
  numi = atoi(argv[2]);
  pthread_mutex_t lock[numt];
  pthread_t pth[numt+1];
  struct timespec time1,time2;
  long int delta;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
  
  int i;
  pthread_t timert;
  data tdata;
  tdata.numt = numt;
  tdata.lock = lock;

  pthread_create(&timert,NULL,timer,&tdata);

  for(i=0; i < numi; i++){
    printf("Iteration %i\n",i+1);
    for (t=0; t < numt;t++){
      pthread_mutex_init(&lock[t],NULL);
      pthread_create(&pth[t],NULL,threadFunc,&lock[t]);
    }

    for (t=0; t < numt;t++)
      pthread_join(pth[t],NULL);

    //for (t=0; t < numt;t++)
    //  pthread_mutex_destroy(&lock[t]);
  }

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);

  delta = diff(time1,time2);
  printf("Avg time taken (ns): %.8f\n",(double)delta/((long)numi*1000000000));
  printf("Avg time taken per iteration (ns): %6.2d\n",delta/(numi*numt*LOOPS));
  
  return 0;
}
