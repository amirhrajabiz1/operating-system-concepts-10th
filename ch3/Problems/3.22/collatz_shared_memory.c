#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number>\n", argv[0]);
    return 1;
  }

  int n = atoi(argv[1]);
  if (n <= 0) {
    printf("Please provide a positive integer.\n");
    return 1;
  }


  const int SIZE = 4096;
  const char *SHARED_MEMORY_NAME = "collatz_shared_memory";

  // Create a shared memory object
  int shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    printf("Shared memory object creation failed. \n");
    return 1;
  }

  // Set the size of the shared memory object
  ftruncate(shm_fd, SIZE);

  // Map the shared memory object into the process's address space
  int *sequence = (int *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (sequence == MAP_FAILED) {
    printf("Shared memory mapping failed.\n");
    return 1;
  }


  pid_t pid = fork();


  if (pid < 0) {
    printf("Fork failed.\n");
    return 1;
  } else if (pid == 0) {
    // Child process
    int index = 0;
    sequence[index++] = n;


    while (n != 1) {
      if (n % 2 == 0) {
	n = n / 2;
      } else {
	n = 3 * n + 1;
      }

      sequence[index++] = n;
    }
  } else {
    // Parent process
    wait(NULL);

    printf("Collatz sequence: ");
    for (int i = 0; sequence[i] != 0; i++) {
      printf("%d ", sequence[i]);
    }
    printf("\n");

    // Remove the shared memory object
    shm_unlink(SHARED_MEMORY_NAME);
  }
  
  // Unmap the shared memory
  munmap(sequence, SIZE);

  return 0;
}

