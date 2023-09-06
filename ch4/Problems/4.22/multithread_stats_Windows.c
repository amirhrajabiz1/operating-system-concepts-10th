#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define NUM_THREADS 3

int num_count = 0;
int *numbers;
int average = 0;
int minimum = 0;
int maximum = 0;

// Function to calculate the average of numbers
DWORD WINAPI calculate_average(LPVOID arg) {
    int sum = 0;
    for (int i = 0; i < num_count; i++) {
        sum += numbers[i];
    }
    average = sum / num_count;
    return 0;
}

// Function to find the minimum value among the numbers
DWORD WINAPI find_minimum(LPVOID arg) {
    minimum = numbers[0];
    for (int i = 1; i < num_count; i++) {
        if (numbers[i] < minimum) {
            minimum = numbers[i];
        }
    }
    return 0;
}

// Function to find the maximum value among the numbers
DWORD WINAPI find_maximum(LPVOID arg) {
    maximum = numbers[0];
    for (int i = 1; i < num_count; i++) {
        if (numbers[i] > maximum) {
            maximum = numbers[i];
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <num1> <num2> ... <numN>\n", argv[0]);
        return 1;
    }

    num_count = argc - 1;
    numbers = (int*)malloc(num_count * sizeof(int));

    // Parse numbers from command line arguments and store in the numbers array
    for (int i = 0; i < num_count; i++) {
        numbers[i] = atoi(argv[i + 1]);
    }

    HANDLE threads[NUM_THREADS];

    // Create three worker threads
    threads[0] = CreateThread(NULL, 0, calculate_average, NULL, 0, NULL);
    threads[1] = CreateThread(NULL, 0, find_minimum, NULL, 0, NULL);
    threads[2] = CreateThread(NULL, 0, find_maximum, NULL, 0, NULL);

    // Wait for all threads to finish
    WaitForMultipleObjects(NUM_THREADS, threads, TRUE, INFINITE);

    // Close thread handles
    for (int i = 0; i < NUM_THREADS; i++) {
        CloseHandle(threads[i]);
    }

    // Output the results
    printf("The average value is %d\n", average);
    printf("The minimum value is %d\n", minimum);
    printf("The maximum value is %d\n", maximum);

    free(numbers); // Free dynamically allocated memory
    return 0;

}
