#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: ./filecopy <input_file> <output_file>\n");
    return 1;
  }

  int pipefd[2];
  pid_t pid;

  // Create the pipe
  if (pipe(pipefd) == -1) {
    perror("pipe");
    return 1;
  }

  // Fork a child process
  pid = fork();

  if (pid < 0) {
    perror("fork");
    return 1;
  } else if (pid == 0) {
    // Child process

    // Close the write end of the pipe
    close(pipefd[1]);

    // Open the destination file for writing
    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1) {
      perror("open");
      return 1;
    }

    // Read from the pipe and write to the destination file
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
      if (write(output_fd, buffer, bytesRead) != bytesRead) {
	perror("write");
	return 1;
      }
    }

    // Close the read end of the pipe and the output file
    close(pipefd[0]);
    close(output_fd);

    return 0;
  } else {
    // Parent process

    // Close the read end of the pipe
    close(pipefd[0]);

    // Open the input file for reading
    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
      perror("open");
      return 1;
    }

    // Read from the input file and write to the pipe
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
      if (write(pipefd[1], buffer, bytesRead) != bytesRead) {
	perror("write");
	return 1;
      }
    }

    // Close the write end of the pipe and the input file
    close(pipefd[1]);
    close(input_fd);

    // Wait for the child process to finish
    wait(NULL);

    return 0;
  }
}
