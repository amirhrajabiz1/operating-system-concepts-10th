#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/* the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {0};

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);

void printArrays() {
  int i;
  int j;
  printf("-----------\n");
  printf("Available:\n");
  for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
    printf("%d ", available[i]);
  }
  printf("\n");

  printf("\n");
  printf("maximum:\n");
  for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
      printf("%d ", maximum[i][j]);
    }
    printf("\n");
  }
  printf("\n");

  printf("\n");
  printf("allocation:\n");
  for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
      printf("%d ", allocation[i][j]);
    }
    printf("\n");
  }
  printf("\n");


  printf("\n");
  printf("need:\n");
  for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
      printf("%d ", need[i][j]);
    }
    printf("\n");
  }
  printf("-----------\n");
  printf("\n");



}



int areCorrespondingElementsSmaller(int arr1[], int arr2[], int size) {
  for (int i = 0; i < size; i++) {
    if (arr1[i] > arr2[i]) {
      return 0; // The first array is not smaller or equat to the second one.
    }
  }
  return 1; // The first array is smaller or equal to the second one.
}

void subtractArrays(int arr1[][NUMBER_OF_RESOURCES], int arr2[][NUMBER_OF_RESOURCES], int result[][NUMBER_OF_RESOURCES], int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      result[i][j] = arr1[i][j] - arr2[i][j];
    }
  }
}

int request_resources(int customer_num, int request[]) {
  // define work
  int work[NUMBER_OF_RESOURCES];
  memcpy(work, available, sizeof(available));
  // define finish
  int finish[NUMBER_OF_CUSTOMERS] = {0};
  // Add the request to the allocation
  if (!areCorrespondingElementsSmaller(request, maximum[customer_num], NUMBER_OF_RESOURCES)){
    printf("You requested more resources than the maximum you told may request.\n");
    return -1;
  }
  for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
    allocation[customer_num][j] += request[j];
  }
  // define need
  subtractArrays(maximum, allocation, need, NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);


  // Safety algorithm
  int is_safe = 0;
  int temp_work[NUMBER_OF_RESOURCES];
  int changed_flag;
  do {
    changed_flag = 0;
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) { 
       if(finish[i] == 0 && areCorrespondingElementsSmaller(need[i], work, NUMBER_OF_RESOURCES)){
	 changed_flag = 1;
         for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
  	 	temp_work[j] = work[j] + allocation[i][j];
         }
         memcpy(work, temp_work, sizeof(temp_work));
         finish[i] = 1;
       }
    }
  } while (changed_flag == 1);

  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    if (finish[i] == 0) {
      is_safe = -1;
      break;
    }
  }

  // if after fulfill the request the state wasn't safe, We will undo the request.
  if (is_safe == -1) {
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
      allocation[customer_num][j] -= request[j];
    }
    subtractArrays(maximum, allocation, need, NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);
  }

  return is_safe;
}

void release_resources(int customer_num, int release[]) {
    if (areCorrespondingElementsSmaller(release, allocation[customer_num], NUMBER_OF_RESOURCES)) {
    	for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
      		allocation[customer_num][j] -= release[j];
    	}
    	subtractArrays(maximum, allocation, need, NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);
	printf("Successfully released resources.\n");
    }
    else {
      printf("We can't release that much resources. So try again...\n");
    }
}

int main(int argc, char *argv[]) {
  int i, j;

  /* start providing available */
  if (argc != NUMBER_OF_RESOURCES + 1) {
    printf("Error: Please provide exactly %d integers as number of resources.\n", NUMBER_OF_RESOURCES);
    return 1;
  }
  // Conveert command-line arguments to integers and store them in the available array
  for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
    available[i] = atoi(argv[i + 1]);
  }
  /* end providing available */
  /* start providing maximum */
  FILE *file;
  char filename[] = "maximum.txt";

  // Open the file
  file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error opening the file %s.\n", filename);
    return 1;
  }

  // Read data from file and store in the maximum array
  for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
      if (fscanf(file, "%d,", &maximum[i][j]) != 1) {
	printf("Error reading data from file. \n");
	fclose(file);
	return 1;
      }
    }
  }
  // Close the file
  fclose(file);
  /* end providing maximum */
  /* start providing request resources */
  char line[100]; // Buffer to hold the input line
  char rqrl[3]; // Variable to hold 'RQ' or 'RL'
  int customer_num; // Variable to hold the customer number
  int resources_request[NUMBER_OF_RESOURCES]; // Array to hold the number of resources a costumer requests.
  int resource_counter = 0; // Counter for the number of requested resources
  int successfull_request;


  // initial need
  for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
    for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
      need[i][j] = maximum[i][j];
    }
  }
  while (1) {
    fgets(line, sizeof(line), stdin); // Read a line from standard input

    if (strchr(line, '*') != NULL) {
      printArrays();
      continue;
    }

    // Remove newline character if present
    size_t len = strlen(line);
    if ( len > 0 && line[len - 1] == '\n'){
      line[len - 1] = '\0';
    }

    // Check if the line is empty or just a newline
    if (strlen(line) <= 1) {
      printf("-----------\n");
      printf("Invalid input Please try again.\n");
      printf("-----------\n\n");
      continue;
    }

    // Tokenize and parse the input line
    char *token = strtok(line, " "); // Get the next token
    if (token == NULL) {
      printf("-----------\n");
      printf("Invalid input format. Please try again.\n");
      printf("-----------\n\n");
      continue;
    }
    strcpy(rqrl, token);

    token = strtok(NULL, " "); // Get the next token
    if (token == NULL || sscanf(token, "%d", &customer_num) != 1) {
      printf("-----------\n");
      printf("Invalid input format. Please try again.\n");
      printf("-----------\n\n");
      continue;
    }

    // Parse the remaining integers
    while ((token = strtok(NULL, " ")) != NULL) { 
      if (isdigit(token[0])) {
	resources_request[resource_counter++] = atoi(token);
	if (resource_counter >= NUMBER_OF_RESOURCES) {
	  break; // Maximum number of resources reached
	}
      }
    }

    if (strcmp(rqrl, "RQ") == 0) {
      printf("-----------\n");
      printf("Handling RQ\n");
      successfull_request = request_resources(customer_num, resources_request);
      if (successfull_request == 0) {
	printf("Successfully requested resources.\n");
      } else if (successfull_request == -1) {
	printf("Failed to request resources.\n");
      }
      printf("-----------\n\n");
    } else if (strcmp(rqrl, "RL") == 0) {
      printf("-----------\n");
      printf("Handling RL\n");
      release_resources(customer_num, resources_request);
      printf("-----------\n\n");
    } else {
      printf("-----------\n");
      printf("Invalid rqrl value. Please try again.\n");
      printf("-----------\n\n");
    }

    resource_counter = 0;
  }

  return 0;
}
