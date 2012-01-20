#include "mvar.h" 

/**
 * newMVar function initializes an MVar with the
 * supplied value, and returns the initialized MVar
 **/

mvar* newMVar(void* arg){
 //create new mvar struct
 mvar* var=(mvar*) malloc(sizeof(mvar));
 pthread_mutex_init(&var->lock,NULL);
 pthread_cond_init(&var->take_cond,NULL);
 pthread_cond_init(&var->put_cond,NULL);
 var->value = arg;
 return (mvar*) var;
}

/**
 * newEmptyMVar function returns an empty MVar 
 **/

mvar* newEmptyMVar(){
 return newMVar(NULL);
}

/**
 * putMVar function tries to put value in MVar.
 * If MVar value is empty, it will go ahead and put
 * the value. Else it will block until value 
 * is empty. On success, it will signal other threads
 * waiting on MVar so that one of them can read the
 * value
 **/

void putMVar(mvar* var,void* value){
  pthread_mutex_lock(&var->lock);
  while(var->value != NULL)
    pthread_cond_wait(&var->put_cond,&var->lock);//if MVar is full, wait until another thread takes the value - release the mutex, and wait on put_cond to become true
  var->value = value;//if here, we got the signal from another thread that took MVar - MVar is empty now. OK to fill
  pthread_cond_signal(&var->take_cond);//signal other threads that value is available for taking now
  pthread_mutex_unlock(&var->lock);
}

/* takeMVar takes the value from an MVar if the value is available
 * Else it blocks until a value is available. Thread scheduling
 * is default OS scheduling
 **/

void* takeMVar(mvar* var){
  void* value;
  pthread_mutex_lock(&var->lock);
  while(var->value == NULL)
    pthread_cond_wait(&var->take_cond,&var->lock);//if MVar is empty, wait until another thread fills it - release the mutex, and wait on take_cond to become true
  //take the value
  value = var->value;
  var->value = NULL; //push NULL value to indicate MVar is empty now
  pthread_cond_signal(&var->put_cond);//signal other threads that value is available for filling now
  pthread_mutex_unlock(&var->lock);
  return value; //return the value that was taken from MVar
}

/**
 * readMVar function reads a value from MVar. That is, it just peeks at it.
 * If no value is available, it will block until the value is available
 **/

void* readMVar(mvar* var){
  void* value;
  pthread_mutex_lock(&var->lock);
  while(var->value == NULL)
    pthread_cond_wait(&var->take_cond,&var->lock);//if MVar is empty, wait until another thread fills it - release the mutex, and wait on take_cond to become true
  //peek at the value
  value = var->value;
  pthread_cond_signal(&var->put_cond);//signal other threads that value is available for filling now
  pthread_mutex_unlock(&var->lock);
  return value; //return the value that was taken from MVar
}


/** 
 * Function to clean up MVar memory stuff before freeing it 
 **/

void deleteMVar(mvar* var){
 pthread_mutex_destroy(&var->lock);
 pthread_cond_destroy(&var->put_cond);
 pthread_cond_destroy(&var->take_cond);
 var->value=NULL;
 free(var);
}
