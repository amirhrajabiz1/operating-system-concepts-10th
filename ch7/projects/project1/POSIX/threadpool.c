/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
task taskQueue[QUEUE_SIZE];
int queueFront = 0;
int queueRear = 0;
int queueCount = 0;


// the worker bee
pthread_t bees[NUMBER_OF_THREADS];
pthread_mutex_t queueMutex;
sem_t queueSemaphore;


// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    pthread_mutex_lock(&queueMutex);

    if (queueCount >= QUEUE_SIZE)
    {
      pthread_mutex_unlock(&queueMutex);
      return 1; // Queue is full
    }

    taskQueue[queueRear] = t;
    queueRear = (queueRear + 1) % QUEUE_SIZE;
    queueCount++;

    pthread_mutex_unlock(&queueMutex);
    sem_post(&queueSemaphore);

    return 0; // Successful insertion
}

// remove a task from the queue
task dequeue() 
{
    sem_wait(&queueSemaphore); // Wait until the queue is not empty
    pthread_mutex_lock(&queueMutex);

    task t = taskQueue[queueFront];
    queueFront = (queueFront + 1) % QUEUE_SIZE;
    queueCount--;

    pthread_mutex_unlock(&queueMutex);

    return t;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    while (TRUE)
    {
      task t = dequeue(); // Get a task from the queue
      execute(t.function, t.data); // Execute the task
    }

    pthread_exit(0);
}


/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
  task t;
  t.function = somefunction;
  t.data = p;

  return enqueue(t);
}

// initialize the thread pool
void pool_init(void)
{
    pthread_mutex_init(&queueMutex, NULL);
    sem_init(&queueSemaphore, 0, 0);

    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
      if (pthread_create(&bees[i], NULL, worker, NULL) != 0)
      {
	perror("Error creating worker thread");
	exit(EXIT_FAILURE);
      }
    }
}

// shutdown the thread pool
void pool_shutdown(void)
{
    // Cancel all worker threads
    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
      if (pthread_cancel(bees[i]) != 0)
      {
	perror("Error canceling worker thread");
      }
    }

    // Join all worker threads
    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
      if (pthread_join(bees[i], NULL) != 0)
      {
	perror("Error joining worker thread");
      }
    }

    pthread_mutex_destroy(&queueMutex); // Destroy the queue mutex
    sem_destroy(&queueSemaphore); // Destroy the queue semaphore
}
