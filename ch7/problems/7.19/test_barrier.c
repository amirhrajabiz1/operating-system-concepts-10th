#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "barrier.c"  // Include the barrier code

#define NUM_THREADS 10

// Function to be performed by each thread
void* thread_function(void* arg) { 
  Barrier* myBarrier = (Barrier*)arg;

  // Do some sork
  printf("Thread %lu doing some work before the barrier.\n",pthread_self());

  // Reach the barrier point
  barrier_point(myBarrier);

  
  // Do more work after the party
  printf("Thread %lu continuing after the barrier.\n", pthread_self());

  return NULL;
}

int main() {
  // Set up the barrier
  Barrier myBarrier;
  if (init(&myBarrier, NUM_THREADS) != 0) {
    fprintf(stderr, "Barrier initialization failed!\n");
    exit(EXIT_FAILURE);
  } else {
    printf("Barrier initialization succeeded.\n");
  }

  // Create threads
  pthread_t threads[NUM_THREADS]; // Creating threads for the test

  for (int i = 0; i < NUM_THREADS; ++i) {
    if (pthread_create(&threads[i], NULL, thread_function, (void*)&myBarrier) != 0) {
      fprintf(stderr, "Thread creation failed!\n");
      exit(EXIT_FAILURE);
    }
  }

  // Wait for all threeads to finish
  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  // Clean up after the party
  destroy(&myBarrier);

  return 0;
}

