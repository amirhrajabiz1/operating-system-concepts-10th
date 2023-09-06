#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

// Define maximum size of the numbers array
#define MAX_SIZE 100  

// Global numbers array
int numbersArray[MAX_SIZE]; 

// Index to keep track of current position in array
int currentIndex = 0;  

// Pointers for first half, second half and result arrays
int *firstHalfArray;
int *secondHalfArray;  
int *resultArray;


// Function to add an integer to the global numbers array
// Returns 0 if added, 1 if array is already full
int addToNumbersArray(int value) {

  // Check if there is space in the array
  if (currentIndex < MAX_SIZE) {
    
    // Add value to the array
    numbersArray[currentIndex] = value;
    
    // Increment index counter
    currentIndex++;
    
    return 0;
  
  // Array is full  
  } else {

    printf("Numbers array is full. Just added the first %i numbers.\n", MAX_SIZE);
    return 1;
  }
}

// Thread function to sort first half of numbers array
void *firstHalfSort(void *args) {

  int j = 0;
  int temp;
  
  // Calculate length of first half
  int lenFirstHalfArray = (currentIndex - currentIndex / 2);

  // Copy first half of numbers array to firstHalfArray
  for (int i = 0; i < currentIndex - currentIndex / 2; i++) {
    firstHalfArray[j] = numbersArray[i];
    j = j + 1;
  }
  
  // Sort the first half array using bubble sort
  for (int i = 0; i < lenFirstHalfArray; i++) {
    for (int j = i + 1; j < lenFirstHalfArray; j++) {
      if (firstHalfArray[i] > firstHalfArray[j]) {
        temp = firstHalfArray[i];
        firstHalfArray[i] = firstHalfArray[j];
        firstHalfArray[j] = temp;
      }
    }
  }
}

// Thread function to sort second half of numbers array  
void *secondHalfSort(void *args) {

  int j = 0;
  int temp;

  // Calculate length of second half
  int lenSecondHalfArray = (currentIndex / 2);

  // Copy second half of numbers array to secondHalfArray
  for (int i = currentIndex - currentIndex / 2; i < currentIndex; i++) {
    secondHalfArray[j] = numbersArray[i];
    j = j + 1;
  }

  // Sort the second half array using bubble sort
  for (int i = 0; i < lenSecondHalfArray; i++) {
    for (int j = i + 1; j < lenSecondHalfArray; j++) {
      if (secondHalfArray[i] > secondHalfArray[j]) {
        temp = secondHalfArray[i];
        secondHalfArray[i] = secondHalfArray[j];
        secondHalfArray[j] = temp;
      }
    }
  }
}

// Thread function to merge the sorted halves into result array
void *mergethread(void *args) {

  int lenFirstArray = currentIndex - currentIndex / 2;
  int lenSecondArray = currentIndex / 2;

  int p = 0;
  int j = 0;

  // Merge the two sorted arrays into result array
  for (int i = 0; i < lenFirstArray; i++) {
    for (; j < lenSecondArray;) {
      if (secondHalfArray[j] <= firstHalfArray[i]) {
        resultArray[p] = secondHalfArray[j];
        p = p + 1;
        j = j + 1;
      }
      else {
        break;
      }
    }
    resultArray[p] = firstHalfArray[i];
    p = p + 1;
  }
  
  // Copy any remaining elements from second array
  for (; j < lenSecondArray; j++) {
    resultArray[p] = secondHalfArray[j];
    p = p + 1; 
  }
}

int main(int argc, char *argv[]) {

  // Check command line arguments
  if (argc < 2) {
    printf("Usage: %s <integer1> <integer2> ...\n", argv[0]);
    return 1;
  }

  // Add command line ints to numbers array
  for (int i = 1; i < argc; i++) {
    int intValue = atoi(argv[i]); 
    int check = addToNumbersArray(intValue);
    if (check == 1) {
      break;
    }
  }

  printf("Integers added to the Numbers array.\n");

  // Allocate memory for temporary arrays
  firstHalfArray = (int *)malloc((currentIndex - currentIndex / 2) * sizeof(int));
  secondHalfArray = (int *)malloc((currentIndex / 2) * sizeof(int));

  // Declare thread IDs
  pthread_t thread1, thread2, thread3;

  // Create thread to sort first half
  if (pthread_create(&thread1, NULL, firstHalfSort, NULL) != 0) {
    perror("pthread_create");
    return 1;
  }

  // Create thread to sort second half
  if (pthread_create(&thread2, NULL, secondHalfSort, NULL) != 0) {
    perror("pthread_create");
    return 1; 
  }

  // Wait for sort threads to finish
  if (pthread_join(thread1, NULL) != 0) {
    perror("pthread_join");
    return 1;
  }
  if (pthread_join(thread2, NULL) != 0) {
    perror("pthread_join");
    return 1;
  }

  // Print sorted arrays (for testing)
  printf("First Half Array:\t");
  for (int i = 0; i < currentIndex - currentIndex / 2; i++) {
    printf("%d ", firstHalfArray[i]);
  }
  printf("\nSecond Half Array:\t");
  for (int i = 0; i < currentIndex / 2; i++) {
    printf("%d ", secondHalfArray[i]);
  }
  printf("\n");

  // Allocate memory for result array
  resultArray = (int *)malloc(currentIndex * sizeof(int));

  // Create thread to merge sorted halves
  if (pthread_create(&thread3, NULL, mergethread, NULL) != 0) {
    perror("pthread_create");
    return 1;
  }

  // Wait for merge thread to finish
  if (pthread_join(thread3, NULL) != 0) {
    perror("pthread_join)"); 
    return 1;
  }

  // Print final sorted result array
  printf("Result Array:\t\t");
  for (int i = 0; i < currentIndex; i++) {
    printf("%d ", resultArray[i]);
  }
  printf("\n");

  // Free allocated memory
  free(firstHalfArray);
  free(secondHalfArray);
  free(resultArray);


  return 0;
}
