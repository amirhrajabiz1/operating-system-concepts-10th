#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define NUM_THREADS 200

// Define a global mutex
pthread_mutex_t mutex;

int  number_of_points_in_circle = 0;

double random_coordinate() {
  // Generate a random double between -1 and 1
  double randomFloat = ((double)rand() / (RAND_MAX / 2.0)) - 1.0;

  return randomFloat;
}

double distance_to_center(double x, double y) {
	double distance = sqrt((x * x) + (y * y));
	return distance;
}

void *calculate_points_in_circle(void *arg) {
  int number_of_all_points = *((int*)arg);
  double x;
  double y;
  double distance;

  for (int i = 0; i < number_of_all_points; i++) {
    x = random_coordinate();
    y = random_coordinate();
    distance = distance_to_center(x, y);
    if (distance <= 1) {
      // Lock the mutex before entering the critical section
      pthread_mutex_lock(&mutex);
      number_of_points_in_circle += 1;
      // Unlock the mutex when done with the critical section
      pthread_mutex_unlock(&mutex);
    }
  }

  pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number_of_points>\n", argv[0]);
    return 1;
  }

  int number_of_all_points = atoi(argv[1]);

  // Seed the random number generator with the current time
  srand((unsigned int)(time(NULL) * clock()));


  pthread_t threads[NUM_THREADS];
  int number_of_all_points_for_each_thread = number_of_all_points / NUM_THREADS;
  // Initialize the mutex
  pthread_mutex_init(&mutex, NULL);

  for (int i = 0; i < NUM_THREADS; i++) {
    // Create One worker thread.
    if (pthread_create(&threads[i], NULL, calculate_points_in_circle, (void*)&number_of_all_points_for_each_thread) != 0) {
      printf("Failed to create the thread.\n");
      return 1;
    }
  }
  
  for (int i = 0; i < NUM_THREADS; i++) {
    // Wait for the thread to finish.
    if (pthread_join(threads[i], NULL) != 0) {
      printf("Failed to joing the thread.\n");
      return 1;
    }
  }

  // Destroy the mutex when it is no longer needed
  pthread_mutex_destroy(&mutex);

 double pi = ((double)number_of_points_in_circle / (double)number_of_all_points) * 4;

 printf("With %i threads and With %i random points and %i points in circle, the π is: %f\n", NUM_THREADS, number_of_all_points, number_of_points_in_circle, pi);
	
 return 0;
}
