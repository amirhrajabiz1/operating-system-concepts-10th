#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define NUM_THREADS 1

// Function to see if a number is prime or not.
bool is_prime(int number) {

  if (number < 2) {
    return false;
  }

  for (int i = 2; i <= number / 2; i++) {
    if (number % i == 0) {
      return false;
    }
  }
  return true;
}

void *show_primes(void *arg) {
  int number = *((int*)arg);
  
  for (int i = 1; i <= number; i++) {
    if (is_prime(i)) {
      printf("%i\t", i);
    }
  }

  printf("\n");
  pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number>\n", argv[0]);
    return 1;
  }
 
  int number = atoi(argv[1]);

  if (number <= 0) {
    printf("Please enter a number greater that '0'\n");
    return 1;
  }


  pthread_t thread;

  // Create One worker thread.
  if (pthread_create(&thread, NULL, show_primes, (void*)&number) != 0) {
    printf("Failed to create the thread.\n");
    return 1;
  }
  
  // Wait for the thread to finish.
  if (pthread_join(thread, NULL) != 0) {
    printf("Failed to join the thread.\n");
    return 1;
  }

  return 0;
}
