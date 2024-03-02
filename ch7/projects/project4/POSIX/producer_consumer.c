#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#include "buffer.h"


typedef struct {
  buffer_item *buffer;	// Array to hold buffer items
  int capacity;		// Maximum capacity of the buffer
  int front;		// Index of the front element
  int rear;		// Index of the rear element
  int count;		// Current number of element in the buffer
} CircularBuffer;

// Function prototypes
CircularBuffer* createBuffer();
void destroyBuffer(CircularBuffer *buffer);
int insertItem(CircularBuffer *buffer, buffer_item item);
int removeItem(CircularBuffer *buffer, buffer_item *item);

// Define full and empty global semaphores
sem_t full;
sem_t empty;
// Define a global mutex
pthread_mutex_t mutex;

// Function to create a circular buffer
CircularBuffer* createBuffer() {
  CircularBuffer *buffer = (CircularBuffer*)malloc(sizeof(CircularBuffer));
  buffer->buffer = (buffer_item*)malloc(sizeof(buffer_item) * BUFFER_SIZE);
  buffer->capacity = BUFFER_SIZE;
  buffer->front = 0;
  buffer->rear = -1;
  buffer->count = 0;
  return buffer;
}

// Function to destroy a circular buffer
void destroyBuffer(CircularBuffer *buffer) {
  free(buffer->buffer);
  free(buffer);
}

// Function to insert an item into the circular buffer
int insertItem(CircularBuffer *buffer, buffer_item item) {
  if (buffer->count >= buffer->capacity) {
    return -1; // Buffer is full
  }
  buffer->rear = (buffer->rear + 1) % buffer->capacity;
  buffer->buffer[buffer->rear] = item;
  buffer->count++;
  return 0; // Successful insertion
}

// Function to remove an item from the circular buffer
int removeItem(CircularBuffer *buffer, buffer_item *item) {
  if (buffer->count <= 0) {
    return -1; // Buffer is empty
  }
  *item = buffer->buffer[buffer->front];
  buffer->front = (buffer->front + 1) % buffer->capacity;
  buffer->count--;
  return 0; // Successful removal
}


void *producer(void *arg) {
  CircularBuffer *buffer = (CircularBuffer *)arg;
  unsigned int item;
  
  while (1) {
    /* sleep for a random period of time */
    sleep((rand() % 5) + 1);
    /* generate a random number */
    item = rand();

    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    if (insertItem(buffer, item)) {
      printf("report error condition.\n");
    }
    else
      printf("producer produced %d.\n", item);
      printf("Ints in buffer %d.\n", buffer->count);

    pthread_mutex_unlock(&mutex);
    sem_post(&full);
  }
}

void *consumer(void *arg) {
  CircularBuffer *buffer = (CircularBuffer *)arg;
  unsigned int item;

  while (1) {
    /* sleep for a random perion of time */
    sleep((rand() % 5) + 1);

    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    if (removeItem(buffer, &item))
      printf("report error condition.\n");
    else
      printf("consumer consumed %d.\n", item);
      printf("Ints in buffer %d.\n", buffer->count);

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
  }
}


int main(int argc, char *argv[]) {
  int sleep_time;
  int num_producers;
  int num_consumers;
  // Create buffer
  CircularBuffer *buffer = createBuffer();

  // Initialize the full and empty semaphores
  sem_init(&empty, 0, BUFFER_SIZE);
  sem_init(&full, 0, 0);

  // Seed the random number generator with the current time
  srand((unsigned int)(time(NULL) * clock()));
  
  // Check if there are exactly three arguments
  if (argc != 4) {
    printf("Usage: %s <program_time> <num_producers> <num_consumers>\n", argv[0]);
    return 1; // Return 1 to indicate error
  }
  sleep_time = atoi(argv[1]);
  num_producers = atoi(argv[2]);
  num_consumers = atoi(argv[3]);

  pthread_t producers[num_producers];
  pthread_t consumers[num_consumers];

  // Create producers and consumers threads
  for (int i = 0; i < num_producers; i++) {
    pthread_create(&producers[i], NULL, producer, buffer);
  }

  for (int i = 0; i < num_consumers; i++) {
    pthread_create(&consumers[i], NULL, consumer, buffer);
  }
     
  // Sleep
  sleep(sleep_time);

  // Destroy te buffer
  destroyBuffer(buffer);

  // Destroy the semaphores
  sem_destroy(&empty);
  sem_destroy(&full);

  // Destroy the mutex
  pthread_mutex_destroy(&mutex);

  // Exit the program
  exit(0);
}


