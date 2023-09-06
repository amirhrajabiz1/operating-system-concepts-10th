#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage %s <command> [arg1] [arg2] ...\n", argv[0]);
    exit(1);
  }

  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(1);
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(1);
  }

  if (pid == 0) {
    // Child process
    close(pipefd[0]);
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    write(pipefd[1], &start_time, sizeof(struct timeval));
    close(pipefd[1]);
    execvp(argv[1], &argv[1]);
    perror("execvp");
    exit(1);
  }

  // Parent process
  close(pipefd[1]);
  int status;
  waitpid(pid, &status, 0);

  struct timeval end_time;
  gettimeofday(&end_time, NULL);

  struct timeval start_time;
  read(pipefd[0], &start_time, sizeof(struct timeval));
  close(pipefd[0]);

  double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
  			(end_time.tv_usec - start_time.tv_usec) / 1000000.0;

  printf("Elapsed time: %.5f\n", elapsed_time);

  return 0;
}
