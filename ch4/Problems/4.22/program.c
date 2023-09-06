#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 3

int num_count = 0;
int *numbers;
int average = 0;
int minimum = 0;
int maximum = 0;

// Function to calculate the average of numbers
void *calculate_average(void *arg) {
  int sum = 0;
  for (int i = 0; i < num_count; i++) {
    sum += numbers[i];
  }
  average = sum / num_count;
  pthread_exit(NULL);
}

// Function to find the minimm value among the numbers
void *find_minimum(void *arg) {
  minimum = numbers[0];
  for (int i = 1; i < num_count; i++) {
    if (numbers[i] < minimum) {
      minimum = numbers[i];
    }
  }
  pthread_exit(NULL);
}

// Function to find the maximum value among the numbers
void *find_maximum(void *arg) {
  maximum = numbers[0];
  for (int i = 1; i < num_count; i++) {
    if (numbers[i] > maximum) {
      maximum = numbers[i];
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <num1> <num2> ... <numN>\n", argv[0]);
    return 1;
  }

  num_count = argc - 1;
  numbers = (int*)malloc(num_count * sizeof(int));

  // Parse numbers from command line arguments and store in the numbers array
  for (int i = 0; i < num_count; i++) {
    numbers[i] = atoi(argv[i + 1]);
  }

  pthread_t threads[NUM_THREADS];

  // Create three worker threads
  pthread_create(&threads[0], NULL, calculate_average, NULL);
  pthread_create(&threads[1], NULL, find_minimum, NULL);
  pthread_create(&threads[2], NULL, find_maximum, NULL);


  // Wait for all threads to finish
  for ( int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  // Output the results
  printf("The average value is %d\n", average);
  printf("The minimum value is %d\n", minimum);
  printf("The maximum value is %d\n", maximum);

  free(numbers); // Free dynamically allocated memory
  return 0;
}
