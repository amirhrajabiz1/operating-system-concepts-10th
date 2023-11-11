#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

// Time quamtum
#define QUANTUM 10

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
  struct node *first = head;        // Pointer to the first node in the list
  struct node *current = NULL;      // Pointer to the current node being processed
  struct node *onetocurrent = NULL; // Pointer to the previous node in the list

  // Check if the list is empty
  if (head == NULL) {
    printf("There is no Task here.\n");
    return;
  }

  // Iterate through the list to schedule and run tasks
  while (first != NULL) {
    current = first;
    onetocurrent = NULL;

    while (current != NULL) {
      Task *task = current->task; // Get the task associated with the current node

      // Check if the remaining burst time of the task is greater than or equal to QUANTUM
      if (task->burst / QUANTUM >= 1) {
        run(task, QUANTUM);             // Run the task for the QUANTUM time
        task->burst = task->burst - QUANTUM; // Update the remaining burst time

      } else if (task->burst % QUANTUM > 0) {
        run(task, task->burst % QUANTUM); // Run the task for the remaining burst time
        task->burst = 0;                 // Set the burst time to zero

        if (current == first) {
          first = first->next; // Move the first pointer to the next task
        } else {
          onetocurrent->next = current->next; // Remove the current task from the list
        }

      } else if (task->burst == 0) {
        if (current == first) {
          first = first->next; // Move the first pointer to the next task
        } else {
          onetocurrent->next = current->next; // Remove the current task from the list
        }
      }

      onetocurrent = current;  // Update the previous node pointer
      current = current->next; // Move to the next node
    }
  }
}

