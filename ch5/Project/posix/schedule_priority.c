#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

// Define a global linked list to store tasks
struct node *head = NULL;

// Function to add a new task to the list
void add(char *name, int priority, int burst) {
  // Allocate memory for a new task
  Task *newTask = (Task *)malloc(sizeof(Task));

  // Set the properties of the new task
  newTask->name = name;
  newTask->priority = priority;
  newTask->burst = burst;

  // Insert the new task into the linked list
  insert(&head, newTask);
}

// Function to schedule and run tasks
void schedule() {
  struct node *first = head; // Initialize a pointer to the first task in the list
  struct node *current = NULL;
  struct node *onetocurrent = NULL;
  struct node *largest = NULL;
  struct node *onetolargest = NULL;

  // Check if the list is empty
  if (head == NULL) {
    printf("There is no Task here.\n"); // Print a message if the list is empty
    return;
  }

  // Main scheduling loop
  while (first->next != NULL) {
    largest = first;
    current = first;
    onetocurrent = NULL;

    // Find the task with the largest priority
    while (current != NULL) {
      if (current->task->priority >= largest->task->priority) {
        largest = current;
        onetolargest = onetocurrent;
      }
      onetocurrent = current;
      current = current->next;
    }

    // Execute the task with the largest priority
    Task *task = largest->task;
    run(task, task->burst);

    // Remove the executed task from the list
    if (onetolargest != NULL) {
      onetolargest->next = largest->next;
    } else {
      first = largest->next;
    }
  }

  // Execute the remaining task (the last one in the list)
  Task *task = first->task;
  run(task, task->burst);
}

