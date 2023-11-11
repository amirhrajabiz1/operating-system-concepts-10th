#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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



// Function to add a new task to the list
void addTask(struct node **largehead, char *name, int priority, int burst) {
  // Allocate memory for a new task
  Task *newTask = (Task *)malloc(sizeof(Task));

  // Set the properties of the new task
  newTask->name = name;
  newTask->priority = priority;
  newTask->burst = burst;

  insert(largehead, newTask);
}

struct node *largehead = NULL;

// Function to schedule and run tasks
void schedule_rr() {
  struct node *first = largehead;        // Pointer to the first node in the list
  struct node *current = NULL;      // Pointer to the current node being processed
  struct node *onetocurrent = NULL; // Pointer to the previous node in the list

  // Check if the list is empty
  if (head == NULL) {
    
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
    onetolargest = NULL;

    // Find the task with the largest priority
    while (current != NULL) {
      if (current->task->priority > largest->task->priority) {
        largest = current;
        onetolargest = onetocurrent;
      }
      onetocurrent = current;
      current = current->next;
    }

    // Remove the executed task from the list
    if (onetolargest != NULL) {
      onetolargest->next = largest->next;
    } else {
      first = largest->next;
    }

    largehead = NULL;
    current = first;
    onetocurrent = NULL;
    // Add the task with the largest priority to the largehead list
    addTask(&largehead, largest->task->name, largest->task->priority, largest->task->burst);

    // Find and add tasks with the same priority as the largest to the largehead list
    while (current != NULL) {
      if (current->task->priority == largest->task->priority) {
        largest = current;
        onetolargest = onetocurrent;
        addTask(&largehead, largest->task->name, largest->task->priority, largest->task->burst);
        
        // Remove the executed task from the list
        if (onetolargest != NULL) {
          onetolargest->next = largest->next;
        } else if(largest->next != NULL) {
          first = largest->next;
        }
      }
      onetocurrent = current;
      current = current->next;
    }

    // Schedule the tasks in largehead using a Round Robin algorithm
    schedule_rr();
  }

  // Execute the remaining task (if any)
  if (largest != first){
    // Execute the last task in the list
    Task *task = first->task;
    run(task, task->burst);
  }
}

