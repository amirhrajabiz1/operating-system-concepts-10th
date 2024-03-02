#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h> // for sleep function

#define MIN_PID 1000000
#define MAX_PID 2000000
#define NUM_THREADS 900000

// Define a global mutex
pthread_mutex_t mutex;

// Bitmap-like data structure to store the availablity of PIDs
unsigned char *pid_map;

// Function prototypes
int allocate_map(void);
int allocate_pid(void);
void release_pid(int pid);

// Function for thread to request and release PIDs
void *pid_manager_thread(void *arg);

int main() {
  if (allocate_map() == 1) {
    printf("PID map allocated successfully.\n");
  } else {
    printf("Failed to allocate PID map.\n");
    return 1;
  }

  pthread_t threads[NUM_THREADS];
  // Initialize the mutex
  pthread_mutex_init(&mutex, NULL);

  // Create multiple threads to simulate PID usage
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, pid_manager_thread, NULL);
  }

  // Wait for all threads to finish
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  // Destroy the mutex when it is no longer needed
  pthread_mutex_destroy(&mutex);
  // Cleanup and free resources
  free(pid_map);

  return 0;
}


// Creates and initializes a data structure for representing PIDs
int allocate_map(void) {
  int range = MAX_PID - MIN_PID + 1;
  int size = (range + CHAR_BIT - 1) / CHAR_BIT; // Calculate the number of bytes needed

  pid_map = (unsigned char *) malloc(size);
  if (pid_map == NULL) {
    return -1;
  }

  for (int i = 0; i < size; i++) {
    pid_map[i] = 0;
  }

  return 1;
}

// Allocates and return a PID
int allocate_pid(void) {
  int range = MAX_PID - MIN_PID + 1;
  int size = (range + CHAR_BIT - 1) / CHAR_BIT;

  for (int i = 0; i < size; i++) {
    if (pid_map[i] != UCHAR_MAX) {
      for (int j = 0; j < CHAR_BIT; j++) {
	unsigned char mask = 1 << j;
	if ((pid_map[i] & mask) == 0) { // Check if the specific bit is available (0)
	  pid_map[i] |= mask; // Set the bit to 1 (allocated)
	  return MIN_PID + i * CHAR_BIT + j;
	}
      }
    }
  }

  return -1; // All PIDs are in use
}


// Release a PID
void release_pid(int pid) {
  if (pid < MIN_PID || pid > MAX_PID) {
    return; // Invalid PID
  }

  int index = (pid - MIN_PID) / CHAR_BIT;
  int position = (pid - MIN_PID) % CHAR_BIT;
  unsigned char mask = ~(1 << position); // Create a mask with a 0 at the position of the PID to release

  pid_map[index] &= mask; // Set the bit to 0 (available)
}

void *pid_manager_thread(void *arg) {
  // Lock the mutex before entering the critical section
  pthread_mutex_lock(&mutex);
  int pid = allocate_pid();
  // Unlock the mutex when done with the critical section
  pthread_mutex_unlock(&mutex);

  if (pid != -1) {
    printf("Thread %ld: Allocated PID: %d\n", (long)pthread_self(), pid);
    int sleep_time = rand() % 5 + 1; // Sleep for a random period (1 to 5 seconds)
    sleep(sleep_time);
    printf("Thread %ld: Released PID: %d\n", (long)pthread_self(), pid);
    release_pid(pid);
  } else {
    printf("Thread %ld: No available PID.\n", (long)pthread_self());
  }

  pthread_exit(NULL);
}
