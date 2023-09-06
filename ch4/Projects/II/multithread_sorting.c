#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 100 // Maximum size of the numbers array

int numbersArray[MAX_SIZE];
int currentIndex = 0; // Index to keep track of the current position in the array
int *firstHalfArray;
int *secondHalfArray;
int *resultArray;


// Function to add an integer to the numbers array
int addToNumbersArray(int value) {
  if (currentIndex < MAX_SIZE) {
    numbersArray[currentIndex] = value;
    currentIndex++;
    return 0;
  } else {
    printf("Numbers array is full. Just added the first %i numbers.\n", MAX_SIZE);
    return 1;
  }
}

void *firstHalfSort(void *args) {
  int j = 0;
  int temp;
  int lenFirstHalfArray = (currentIndex - currentIndex / 2);
  for (int i = 0; i < currentIndex - currentIndex / 2; i++) {
    firstHalfArray[j] = numbersArray[i];
    j = j + 1;
  }
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
 

void *secondHalfSort(void *args) {
  int j = 0;
  int temp;
  int lenSecondHalfArray = (currentIndex / 2);
  for (int i = currentIndex - currentIndex / 2; i < currentIndex; i++) {
    secondHalfArray[j] = numbersArray[i];
    j = j + 1;
  }
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

void *mergethread(void *args) {
  int lenFirstArray = currentIndex - currentIndex / 2;
  int lenSecondArray = currentIndex / 2;

  int p = 0;
  int j = 0;
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
  for (; j < lenSecondArray; j++) {
    resultArray[p] = secondHalfArray[j];
    p = p + 1;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <integer1> <integer2> ...\n", argv[0]);
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    int intValue = atoi(argv[i]); // Convert command line argument to an integer.
    int check = 0; // Check if the index of the number is less than the MAX_SIZE
    check = addToNumbersArray(intValue);
    if (check == 1) {
      break;
    }
  }
  printf("Integers added to the Numbers array.\n");


  firstHalfArray = (int *)malloc((currentIndex - currentIndex / 2) * sizeof(int));
  secondHalfArray = (int *)malloc((currentIndex / 2) * sizeof(int));

  pthread_t thread1, thread2, thread3;

  // Create the first thread
  if (pthread_create(&thread1, NULL, firstHalfSort, NULL) != 0) {
    perror("pthread_create");
    return 1;
  }

  // Create the second thread
  if (pthread_create(&thread2, NULL, secondHalfSort, NULL) != 0) {
    perror("pthread_create");
    return 1;
  }

  // Wait for both threads to finish
  if (pthread_join(thread1, NULL) != 0) {
    perror("pthread_join");
    return 1;
  }
  if (pthread_join(thread2, NULL) != 0) {
    perror("pthread_join");
    return 1;
  }

  // For test
  printf("First Half Array:\t");
  for (int i = 0; i < currentIndex - currentIndex / 2; i++) {
    printf("%d ", firstHalfArray[i]);
  }
  printf("\nSecond Half Array:\t");
  for (int i = 0; i < currentIndex / 2; i++) {
    printf("%d ", secondHalfArray[i]);
  }
  printf("\n");

  
  // producing the result array

  resultArray = (int *)malloc(currentIndex * sizeof(int));

  // Create the merging thread  
  if (pthread_create(&thread3, NULL, mergethread, NULL) != 0) {
    perror("pthread_create");
    return 1;
  }

  // Wait for merge thread to finish
  if (pthread_join(thread3, NULL) != 0) {
    perror("pthread_join)");
    return 1;
  }

  // Print the result array
  printf("Result Array:\t\t");
  for (int i = 0; i < currentIndex; i++) {
    printf("%d ", resultArray[i]);
  }
  printf("\n");

  return 0;
}
