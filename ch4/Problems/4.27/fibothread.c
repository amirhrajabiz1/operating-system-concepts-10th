#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define the maximum size for the Fibonacci sequence.
#define MAX_SIZE 100

// The shared data structure.
int shared_data[MAX_SIZE];

// The Fibonacci sequence generation function to be run in a separate thread.
void* generate_fibonacci(void* arg) {
  int n = *((int*)arg);

  // The first two numbers in the Fibonacci sequence are always 0 and 1.
  shared_data[0] = 0;
  shared_data[1] = 1;

  // Generate the rest of the Fibonacci sequence.
  for(int i = 2; i < n; i++) {
    shared_data[i] = shared_data[i-1] + shared_data[i-2];
  }

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

  // Create a separate thread to generate the Fibonacci sequence.
  rc = pthread_create(&tid, NULL, generate_fibonacci, &n);
  if(rc != 0) {
    fprintf(stderr, "ERROR: Failed to create thread.\n");
    return EXIT_FAILURE;
  }

  // Wait for the child thread to finish.
  rc = pthread_join(tid, NULL);
  if(rc != 0) {
    fprintf(stderr, "ERROR: Failed to join thread.\n");
    return EXIT_FAILURE;
  }

  // Print the Fibonacci sequence
  for(int i = 0; i < n; i++) {
    printf("%d\t", shared_data[i]);
  }
  printf("\n");

  return EXIT_SUCCESS;
}
