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
  struct node *first = head;
  struct node *tail = NULL;
  struct node *onetotail = NULL;

  // Check if the list is empty
  if (head == NULL) {
    printf("There is no Task here.\n");
    return;
  }

  // Iterate through the list to schedule and run tasks
  while (first->next != NULL)
  {
        onetotail = first;

        // Move 'tail' to the end of the list
        while (onetotail != NULL) {
                tail = onetotail->next;
                if (tail->next == NULL){
                        // Get the task from 'tail' and run it
                        Task *task = tail->task;
                        run(task, task->burst);
                        // Remove the 'tail' from the list
                        onetotail->next = NULL;
                }
                onetotail = onetotail->next;
        }
  }

  // Run the task at the 'first' node (there should be one task remaining)
  Task *task = first->task;
  run(task, task->burst);
}

