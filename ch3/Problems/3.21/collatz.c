#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


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

  pid_t pid = fork();


  if (pid < 0) {
    printf("Fork failed.\n");
    return 1;
  } else if (pid == 0) {
    // Child process
    printf("%d ", n);


    while (n != 1) {
      if (n % 2 == 0) {
	n = n / 2;
      } else {
	n = 3 * n + 1;
      }

      printf("%d ", n);
    }
  } else {
    // Parent process
    wait(NULL);
    printf("\nChild process completed.\n");
  }

  return 0;
}

