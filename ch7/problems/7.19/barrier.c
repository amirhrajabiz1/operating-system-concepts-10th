#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int count;
  int size;
} Barrier;

int init(Barrier *barrier, int n) {
  barrier->count = 0;
  barrier->size = n;

  if (pthread_mutex_init(&barrier->mutex, NULL) != 0 ||
      pthread_cond_init(&barrier->cond, NULL) != 0) {
      return -1; // Oops, error initializing
  }

  return 0; // All set, no errors
}

int barrier_point(Barrier *barrier) {
  pthread_mutex_lock(&barrier->mutex);

  barrier->count++;

  if (barrier->count == barrier->size) {
    barrier->count = 0;
    pthread_cond_broadcast(&barrier->cond);
  } else {
    pthread_cond_wait(&barrier->cond, &barrier->mutex);
  }

  pthread_mutex_unlock(&barrier->mutex);

  return 0; // No errors, we hope
}

// Cleanup function
void destroy(Barrier *barrier) {
  pthread_mutex_destroy(&barrier->mutex);
  pthread_cond_destroy(&barrier->cond);
}
