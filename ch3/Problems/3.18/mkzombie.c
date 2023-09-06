#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>


int main() {
  	pid_t childPid;


	childPid = fork();

	
	if (childPid == -1) {
	  perror("fork");
	  return 1;
	} else if (childPid == 0) {
	  // Child process
	  char *args[] = {"ls", "-ltrh", NULL};
	  execvp("ls", args);

	  
	  // execvp only return if an error occurs
	  perror("execvp");
	  exit(1);
	} else {
	  // Parent process
	  printf("Child Process created. It will be a zombie.\n");


	  sleep(30); // Sleep for 30 seconds


	  printf("Parent process exiting.\n");
	  exit(0);
	}

	return 0;
}
