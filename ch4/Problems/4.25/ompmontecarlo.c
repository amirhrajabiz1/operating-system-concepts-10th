#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>


#define NUM_THREADS 2

int number_of_points_in_circle = 0;

double random_coordinate() {
  // Generate a random double between -1 and 1
  double randomFloat = ((double)rand() / (RAND_MAX / 2.0)) -1.0;

  return randomFloat;
}

double distance_to_center(double x, double y) {
  double distance = sqrt((x * x) + (y * y));
  return distance;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number_of_points>\n", argv[0]);
    return 1;
  }

  int number_of_all_points = atoi(argv[1]);

  // Seed the random number generator with the current time
  srand((unsigned int)(time(NULL) * clock()));

  // set the number of threads to use

  omp_set_num_threads(NUM_THREADS);

  // Start measuring time
  double start_time = omp_get_wtime();

  // Create a team of threads that will generate random points in parallel
#pragma omp parallel for reduction(+:number_of_points_in_circle)
  for (int i = 0; i < number_of_all_points; i++) {
    double x = random_coordinate();
    double y = random_coordinate();
    double distance = distance_to_center(x, y);
    if (distance <= 1) {
      number_of_points_in_circle += 1;
    }
  }

  // Stop measuring time
  double end_time = omp_get_wtime();

  double pi = ((double) number_of_points_in_circle / (double)number_of_all_points) * 4;

  printf("With %i random points and %i points in circle, the π is: %f\n", number_of_all_points, number_of_points_in_circle, pi);
  printf("The program took %f seconds to run.\n", end_time - start_time);

  return 0;

}
