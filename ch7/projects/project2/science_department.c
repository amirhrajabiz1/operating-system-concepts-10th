#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // for sleep()
#include <time.h>

// Define constants
# define NUM_STUDENTS 7
# define MAX_CAPACITY 3

// Node structure for linked list
typedef struct Node {
  pthread_t thread_id;
  struct Node* next;
} Node;

// Global variables
Node* head = NULL; // Head of the linked list
Node* tail = NULL; // Tail of the linked list
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for thread safety
sem_t ta_semaphore; // Semaphore to signal TA
sem_t student_semaphore; // Semaphore to control student access
int working_with_ta_time; // Time a student works with the TA
struct tm *timeinfo; // Time variable

// Function to update time
void updateTime() {
  time_t rawtime;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
}

// Function to add a student thread to the linked list
int add_thread(pthread_t thread_id) {
  // Check if queue has reached maximum capacity
  int queue_size = 0;
  Node* current = head;
  while (current != NULL) {
    queue_size++;
    current = current->next;
  }
  if (queue_size >= MAX_CAPACITY) {
    // The queue is full
    return -1;
  }

  Node* new_node = (Node*)malloc(sizeof(Node));
  new_node->thread_id = thread_id;
  new_node->next = NULL;

  if (head == NULL) {
    head = new_node;
    tail = new_node;
  } else {
    tail->next = new_node;
    tail = new_node;
  }
  return 0;
}

// Function to remove a thread from the linked list
long int remove_thread() {
  if (head != NULL) {
    Node* temp = head;
    head = head-> next;
    long int student_id = temp->thread_id;
    free(temp);
    return student_id;
  }
}

// Student function
void *student_function(void *arg) {
  int queue_check;
  int sleep_time;
  while (1){
    pthread_mutex_lock(&mutex);
    queue_check = add_thread(pthread_self());
    pthread_mutex_unlock(&mutex);
    if (queue_check == -1) {
      // Generate a random sleep time between 1 and 10 seconds
      sleep_time = (rand() % 10) + 1;
      updateTime();
      printf("%02d:%02d:%02d: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
      printf("I'm the student number '%li'. The queue is full so I'll do programming for another %i seconds.\n",pthread_self(), sleep_time);
      // Sleep for the random amount of time
      sleep(sleep_time);
    } else {
      updateTime();
      printf("%02d:%02d:%02d: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
      printf("I'm the student number '%li'. The queue has empty space, so Now I'm going to the queue.\n", pthread_self());
      sem_wait(&student_semaphore); // Wait if the room if full
      working_with_ta_time = (rand() % 10) + 1;
      sem_post(&ta_semaphore); // Signal the ta_function
      sleep(working_with_ta_time);
      break;
    }
  }
  pthread_exit(NULL);
}

// TA function
void *ta_function(void *arg) {
  long int student_id;
  int count_of_saw_students = 0;
  while(1) {
    sem_wait(&ta_semaphore); // Wait for signal from student_function
    count_of_saw_students++;
    pthread_mutex_lock(&mutex);
    student_id = remove_thread();
    pthread_mutex_unlock(&mutex);
    updateTime();
    printf("%02d:%02d:%02d: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    printf("I'm TA, I will practice with student number '%li' for %i seconds. \n", student_id, working_with_ta_time);
    sleep(working_with_ta_time);
    updateTime();
    printf("%02d:%02d:%02d: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    printf("I'm TA, I practiced with student number '%li' for %i seconds. \n", student_id, working_with_ta_time);
    sem_post(&student_semaphore); // tell to the head student in the queue which the room is empty
    if (count_of_saw_students == NUM_STUDENTS) {
      break;
    }
  }
  pthread_exit(NULL);
}

// Main function
int main(void)
{
  sem_init(&ta_semaphore, 0, 0);
  sem_init(&student_semaphore, 0, 1);
  pthread_t student_threads[NUM_STUDENTS];
  pthread_t ta_thread;
  int rc, i;

  // Seed the random number generator with the current time
  srand((unsigned int)(time(NULL) * clock()));

  updateTime();
  printf("%02d:%02d:%02d: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  printf("Now we open the department. We have %i students and we have %i chairs in the queue.\n", NUM_STUDENTS, MAX_CAPACITY);

  // Create the TA thread
  rc = pthread_create(&ta_thread, NULL, ta_function, NULL);
  if (rc) {
    printf("ERROR: return from pthread_create() is %d\n", rc);
    exit(-1);
  }

  // Create students threads
  for (i = 0; i < NUM_STUDENTS; ++i)
  {
    rc = pthread_create(&student_threads[i], NULL, student_function, NULL);
    if (rc) {
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  // Wait for students to finish
  for (i = 0; i < NUM_STUDENTS; ++i)
  {
    pthread_join(student_threads[i], NULL);
  }

  // Wait for TA to finish
  pthread_join(ta_thread, NULL);

  // Destroy semaphores
  sem_destroy(&ta_semaphore);
  sem_destroy(&student_semaphore);

  updateTime();
  printf("%02d:%02d:%02d: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  printf("Now We close the department.\n");

  return 0;
}

