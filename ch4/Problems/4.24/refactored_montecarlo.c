#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define NUM_THREADS 1

typedef struct {
  int num_points;
  int points_in_circle;
} ThreadArgs;

double random_coordinate() {
  // Denerate a random double between -1 and 1
  return ((double)rand() / (RAND_MAX / 2.0)) - 1.0;
}

double distance_to_center(double x, double y) {
  return sqrt((x * x) + (y * y));
}

void *calculate_points_in_circle(void *arg) {
  ThreadArgs* args = (ThreadArgs*)arg;
  double x, y, distance;

  for (int i = 0; i < args->num_points; i++) {
    x = random_coordinate();
    y = random_coordinate();
    distance = distance_to_center(x, y);
    if (distance <= 1) {
      args->points_in_circle++;
    }
  }

  pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number_of_points>\n", argv[0]);
    return 1;
  }

  int total_points = atoi(argv[1]);

  // Seed the random number generator with the current time
  struct timeval time;
  gettimeofday(&time, NULL);
  srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


  pthread_t thread;
  ThreadArgs thread_args;
  thread_args.num_points = total_points;
  thread_args.points_in_circle = 0;

  // Create One worker thread.
  if (pthread_create(&thread, NULL, calculate_points_in_circle, &thread_args) != 0) {
    printf("Failed to create the thread.\n");
    return 1;
  }

  // Wait for the thread to finish.
  if (pthread_join(thread, NULL) != 0) {
    printf("Failed to joing the thread.\n");
    return 1;
  }

 double pi_estimate = ((double)thread_args.points_in_circle / (double)total_points) * 4;

 printf("With %i random points and %i points in the circle, the π is: %f\n", total_points, thread_args.points_in_circle, pi_estimate);
	
 return 0;
}
