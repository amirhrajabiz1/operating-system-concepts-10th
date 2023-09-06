#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>


#define BUFFER_SIZE 100

void reverseCase(char *str) {
  for (int i = 0; i < strlen(str); i++) {
    if (islower(str[i]))
      str[i] = toupper(str[i]);
    else if (isupper(str[i]))
      str[i] = tolower(str[i]);
  }
}

int main() {
  int pipe1[2]; // Pipe for sending the original message
  int pipe2[2]; // Pipe for sending the modified message


  pid_t pid;

  if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
    perror("Pipe creation failed");
    exit(EXIT_FAILURE);
  }

  pid = fork();

  if (pid < 0) {
    perror("Fork failed");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // Chiled process (second process)

    close(pipe1[1]); // Close write end of pipe1
    close(pipe2[0]); // Close read end of pipe2

    char receivedMessage[BUFFER_SIZE];
    read(pipe1[0], receivedMessage, BUFFER_SIZE); // Read the message from pipe1

    reverseCase(receivedMessage); // Reverse the case of each character

    write(pipe2[1], receivedMessage, strlen(receivedMessage) + 1); // Write the modified message to pipe2

    close(pipe1[0]);
    close(pipe2[1]);
  } else {
    // Parent process (first process)

    close(pipe1[0]); // Close read end of pipe1
    close(pipe2[1]); // Close write end of pipe2

    char originalMessage[BUFFER_SIZE] = "Hi There";
    char modifiedMessage[BUFFER_SIZE];

    write(pipe1[1], originalMessage, strlen(originalMessage) + 1); // Write the original message to pipe1

    read(pipe2[0], modifiedMessage, BUFFER_SIZE); // Read the modified message from pipe2

    printf("Modified Message: %s\n", modifiedMessage);

    close(pipe1[1]); // Close write end of pipe1
    close(pipe2[0]); // Close read end of pipe2
  }
  return 0;
}
