#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define NUM_THREADS 1

int  number_of_points_in_circle = 0;

double random_coordinate() {
  // Denerate a random double between -1 and 1
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
      number_of_points_in_circle += 1;
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


  pthread_t thread;

  // Create One worker thread.
  if (pthread_create(&thread, NULL, calculate_points_in_circle, (void*)&number_of_all_points) != 0) {
    printf("Failed to create the thread.\n");
    return 1;
  }

  // Wait for the thread to finish.
  if (pthread_join(thread, NULL) != 0) {
    printf("Failed to joing the thread.\n");
    return 1;
  }

 double pi = ((double)number_of_points_in_circle / (double)number_of_all_points) * 4;

 printf("With %i random points and %i points in circle, the π is: %f\n", number_of_all_points, number_of_points_in_circle, pi);
	
 return 0;
}
