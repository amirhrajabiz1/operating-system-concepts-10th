#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define NUM_PHILOSOPHERS 5

pthread_t philosophers[NUM_PHILOSOPHERS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t time_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
int current_philosopher_index = 0;

enum {
  THINKING,
  HUNGRY,
  EATING
} state[NUM_PHILOSOPHERS];

// Function to update time
void updateShowTime() {
  struct tm timeinfo; 
  time_t rawtime;
  char buffer[20]; // Buffer for formatted time

  time(&rawtime);
  localtime_r(&rawtime, &timeinfo); // Thread-safe version of localtime
  strftime(buffer, sizeof(buffer), "%H:%M:%S: ", &timeinfo); // Format time as HH:MM:SS
  buffer[sizeof(buffer) - 1] = '\0'; // Ensure null termination

  ssize_t length =  write(STDOUT_FILENO, buffer, strlen(buffer)); // Write to standard output

  if (length < 0) {
    perror("Error writing to STDOUT_FILENO");
    exit(EXIT_FAILURE);
  }
}


void pickup_forks(int philosopher_id) {
  while ((state[(philosopher_id + (NUM_PHILOSOPHERS - 1)) % NUM_PHILOSOPHERS] == EATING) || (state[(philosopher_id + 1) % NUM_PHILOSOPHERS] == EATING)) {
    pthread_cond_wait(&condition, &mutex);
  }
  pthread_mutex_lock(&time_mutex);
  updateShowTime();
  printf("I'm philosopher number '%i', and I'm picking up my forks.\n", philosopher_id);
  pthread_mutex_unlock(&time_mutex);
  return;
}

void return_forks(int philosopher_id) {
  pthread_mutex_lock(&time_mutex);
  updateShowTime();
  printf("I'm philosopher number '%i', and I'm returning forks.\n", philosopher_id);
  pthread_mutex_unlock(&time_mutex);
  state[philosopher_id] = THINKING;
  pthread_cond_broadcast(&condition);
  return;
}


void* philosopher_function(void* philoshopher_id) {
  int tid = *(int*)philoshopher_id;
  int sleep_time;
  while (1) {
      pthread_mutex_lock(&mutex);
      pickup_forks(tid);
      state[tid] = EATING;
      pthread_mutex_unlock(&mutex);
      sleep_time = (rand() % 3) + 1;
      pthread_mutex_lock(&time_mutex);
      updateShowTime();
      printf("I'm philosopher number '%i', and I'll eat now for %i seconds.\n", tid, sleep_time);
      pthread_mutex_unlock(&time_mutex);
      sleep(sleep_time);
      return_forks(tid);
      sleep_time = (rand() % 3) + 1;
      pthread_mutex_lock(&time_mutex);
      updateShowTime();
      printf("I'm philosopher number '%i', and I'll think now for %i seconds.\n", tid, sleep_time);
      pthread_mutex_unlock(&time_mutex);
      sleep(sleep_time);
      pthread_mutex_lock(&time_mutex);
      updateShowTime();
      printf("I'm philosopher number '%i', and I'm hungry now.\n", tid);
      pthread_mutex_unlock(&time_mutex);
    }
  return NULL;
}

int main() {
  int i, philosopher_id[NUM_PHILOSOPHERS];

  // Seed the random number generator with the current time
  srand((unsigned int)(time(NULL) * clock()));

  // Create threads
  for (i = 0; i < NUM_PHILOSOPHERS; i++) {
    philosopher_id[i] = i;
    if (pthread_create(&philosophers[i], NULL, philosopher_function, (void*)&philosopher_id[i]) != 0) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }
    state[i] = THINKING;
  }

  // Join threads
  for (i = 0; i < NUM_PHILOSOPHERS; i++) {
    if (pthread_join(philosophers[i], NULL) != 0) {
      perror("pthread_join");
      exit(EXIT_FAILURE);
    }
  }

  return 0;
}
      



