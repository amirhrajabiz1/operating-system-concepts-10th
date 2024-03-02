#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define the maximum size for the Fibonacci sequence.
#define MAX_SIZE 20

// The shared data structure.
int shared_data[MAX_SIZE];

// Define a global mutex
pthread_mutex_t mutex;
// Define a global condition variable
pthread_cond_t cond_var;

// The Fibonacci sequence generation function to be run in a separate thread.
void* generate_fibonacci(void* arg) {
  // Lock the mutex before entering the critical section
  pthread_mutex_lock(&mutex);
  int n = *((int*)arg);

  // The first two numbers in the Fibonacci sequence are always 0 and 1.
  shared_data[0] = 0;
  shared_data[1] = 1;

  // Generate the rest of the Fibonacci sequence.
  for(int i = 2; i < n; i++) {
    shared_data[i] = shared_data[i-1] + shared_data[i-2];
  }

  // Signal to the waiting thread on the condition variable.
  pthread_cond_signal(&cond_var);
  // Unlock the mutex when done with the critical section.
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: %s <n>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int n = atoi(argv[1]);

  if(n <= 0 || n > MAX_SIZE) {
    printf("Please enter a positive integer less that or equal to %d.\n", MAX_SIZE);
    return EXIT_FAILURE;
  }

  pthread_t tid;
  int rc;

  // Initialize the mutex
  pthread_mutex_init(&mutex, NULL);
  // Initialize the condition variable
  pthread_cond_init(&cond_var, NULL);

  // Create a separate thread to generate the Fibonacci sequence.
  rc = pthread_create(&tid, NULL, generate_fibonacci, &n);
  if(rc != 0) {
    fprintf(stderr, "ERROR: Failed to create thread.\n");
    return EXIT_FAILURE;
  }

  // Lock the mutex
  pthread_mutex_lock(&mutex);
  // This line put this thread to wait set and realease the mutex and wait until a signal on cond_var.
  pthread_cond_wait(&cond_var, &mutex);
  // Print the Fibonacci sequence
  for(int i = 0; i < n; i++) {
    printf("%d\t", shared_data[i]);
  }
  // Unlock the mutex
  pthread_mutex_unlock(&mutex);
  printf("\n");

  // Wait for the child thread to finish.
  rc = pthread_join(tid, NULL);
  if(rc != 0) {
    fprintf(stderr, "ERROR: Failed to join thread.\n");
    return EXIT_FAILURE;
  }

  // Destroy the mutex when it is no longer needed
  pthread_mutex_destroy(&mutex);
  // Destroy the condition variable 
  pthread_cond_destroy(&cond_var);

  return EXIT_SUCCESS;
}
