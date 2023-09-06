#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAX_LINE 80
#define HISTORY_SIZE 10
#define SHM_NAME "/pipe_shm"
#define SHM_SIZE 4096

void parse_input(char *input, char **args, int *background, char **input_file, char **output_file);
// Parses the user input into individual arguments and determines if the command should be run in the background.

void add_to_history(char *input, char **history, int *history_count);
// Adds the user input to the command history array.

void execute_previous_command(char **args, char **history, int *history_countchar, char *input_file, char *output_file);
// Executes the previous command from the command history.

void execute_command(char **args, int background, char *input_file, char *output_file);
// Executes the given command with the specified arguments.

void save_history_to_file(char **history, int history_count);
// Write the history to the ./.history file.

void load_history_from_file(char **history, int *history_count);
// Load from the ./.history file to history array.

void show_history(char ** history, int history_count);

void split_pipe_commands(char *input, char **commands);

void handle_pipe_commands(char **pipe_commands, int background, char **history, int history_count);
// Executes a single pipe commands with shared memory.

void execute_pipe_command(char **args, int read_fd, int write_fd);
// Executes a single pipe command with the given arguments.

int main(void)
{
    char *args[MAX_LINE / 2 + 1];
    int should_run = 1;
    int background;
    char *history[HISTORY_SIZE];
    int history_count = 0;

    load_history_from_file(history, &history_count);

    char *input_file = NULL;
    char *output_file = NULL;

    while (should_run)
    {
        printf("ashell>"); // Prompt the user for input
        fflush(stdout);

        char input[MAX_LINE + 1];
        fgets(input, sizeof(input), stdin); // Read user input

        if (strcmp(input, "!!\n") == 0) // Check if user input is '!!' to execute previous command
        {
            execute_previous_command(args, history, &history_count, input_file, output_file);
            continue;
        }

	char *pipe_commands[MAX_LINE / 2 + 1];
	split_pipe_commands(input, pipe_commands); // Split the input into pipe_command array

	if (pipe_commands[1] != NULL) // Check if there are pipe commands
	{
	  handle_pipe_commands(pipe_commands, background, history, history_count);
	  continue;
	}

        add_to_history(input, history, &history_count); // Add user input to command history
        parse_input(input, args, &background, &input_file, &output_file); // Parse the user input into individual arguments

        if (strcmp(args[0], "exit") == 0) // Check if user wants to exit the shell
        {
            should_run = 0;
	    save_history_to_file(history, history_count); // Save the history to file before exiting.
            continue;
        }
        else if ((strcmp(args[0], "") == 0) && args[1] == NULL) // Check if user entered an empty command
        {
            continue;
        } 
	else if (strcmp(args[0], "history") == 0) // Check if user wants to view the shell history
	{
	  show_history(history, history_count);
	  continue;
	}

        execute_command(args, background, input_file, output_file); // Execute the user command
    }

    return 0;
}

void split_pipe_commands(char *input, char **commands)
{
  int i = 0;
  char *token = strtok(input, "|");

  while (token != NULL)
  {
    commands[i++] = token;
    token = strtok(NULL, "|");
  }

  commands[i] = NULL;
}

void parse_input(char *input, char **args, int *background, char **input_file, char **output_file)
{
    int i = 0;
    char *token = strtok(input, " \n");
    *background = 0;
    *input_file = NULL;
    *output_file = NULL;

    while (token != NULL)
    {
        if (strcmp(token, "&") == 0) // Check if the argument is '&', indicating background execution
        {
            *background = 1;
        }
	else if (strcmp(token, "<") == 0) // Check the input redirection
	{
	  token = strtok(NULL, " \n");
	  *input_file = token;
	}
	else if (strcmp(token, ">") == 0) // Check for the output redirection
	{
	  token = strtok(NULL, " \n");
	  *output_file = token;
	}
        else
        {
            args[i++] = token; // Store the argument in the argument array
        }

        token = strtok(NULL, " \n"); // Move to the next argument
    }

    args[i] = NULL; // Add a NULL terminator at the end of the argument array

    if (i == 0)
    {
        args[0] = ""; // If no arguments are provided, set the first argument to an empty string
        args[1] = NULL;
    }
}

void add_to_history(char *input, char **history, int *history_count)
{
    if (*history_count < HISTORY_SIZE) // Check if the history array is not full
    {
        history[*history_count] = strdup(input); // Add a duplicate of the input to the history array
        (*history_count)++; // Increment the history count
    }
    else
    {
        free(history[0]); // Free the memory of the oldest command in the history
        for (int i = 0; i < HISTORY_SIZE - 1; i++)
        {
            history[i] = history[i + 1]; // Shift the history array to the left
        }
        history[HISTORY_SIZE - 1] = strdup(input); // Add the new command to the end of the history array
    }
}

void execute_previous_command(char **args, char **history, int *history_count, char *input_file, char *output_file)
{
    if (*history_count > 0) // Check if there are commands in the history
    {
        (*history_count)--; // Decrement the history count to get the index of the previous command
        char *previous_command = history[*history_count]; // Get the previous command from the history
        printf("Executing previous command: %s", previous_command);
        int background = 0;
        memset(args, 0, sizeof(args)); // Clear the arguments array
        parse_input(previous_command, args, &background, &input_file, &output_file); // Parse the previous command into arguments
        execute_command(args, background, input_file, output_file); // Execute the previous command
        free(previous_command); // Free the memory of the previous command
    }
    else
    {
        printf("No previous command in history.\n");
    }
}

void execute_command(char **args, int background, char *input_file, char *output_file)
{
    pid_t pid = fork(); // Fork a new process

    if (pid == 0) // Child process
    {
      if (input_file != NULL) // Handle input redirection
      {
	int fd = open(input_file, O_RDONLY);
	if (fd == -1)
	{
	  perror("open input file");
	  exit(1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
      }
      
      if (output_file != NULL) // Hangle output redirection
      {
	int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
	  perror("open output file");
	  exit(1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
      }

        execvp(args[0], args); // Execute the command with the given arguments
        perror("execvp"); // Print an error message if execution fails
        exit(1); // Exit the child process with an error status
    }
    else if (pid > 0) // Parent process
    {
        if (!background) // If the command is not meant to be run in the background
        {
            wait(NULL); // Wait for the child process to complete
        }
    }
    else // Fork failed
    {
        perror("fork"); // Print an error message
        exit(1); // Exit the program with an error status
    }
}

void save_history_to_file(char **history, int history_count)
{
    FILE *file = fopen("./.history", "w");
    if (file == NULL)
    {
        perror("Unable to open file for writing");
        return;
    }

    int start_index = 0;
    for (int i = start_index; i < history_count; i++)
    {
        fprintf(file, "%d: %s", i + 1, history[i]);
    }

    fclose(file);
}


void load_history_from_file(char **history, int *history_count)
{
  FILE *file = fopen("./.history", "r");
  if (file == NULL)
  {
    perror("Unable to open file for reading");
    return;
  }

  char line[MAX_LINE + 1];
  int line_index = 1; // Initialinze line index

  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (*history_count >= HISTORY_SIZE)
    {
      free(history[0]);
      for (int i = 0; i < HISTORY_SIZE - 1; i++)
      {
	history[i] = history[i + 1];
      }
      (*history_count)--;
    }

    // Remove the line index from the line
    char *command = strchr(line, ':');
    if (command != NULL)
    {
      command++; //Move past the ':' character
      history[*history_count] = strdup(command);
      (*history_count)++;
    }

    line_index++;
  }

  fclose(file);
}

void show_history(char **history, int history_count)
{
  printf("Shell History:\n");
  for (int i = 0; i < history_count; i++)
  {
    printf("%d: %s", i + 1, history[i]);
  }
}

void handle_pipe_commands(char **pipe_commands, int background, char **history, int history_count)
{
  int pipe_fds[2];
  int read_fd = -1;

  // Create shared memory
  int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1)
  {
    perror("shm_open");
    exit(1);
  }

  if (ftruncate(shm_fd, SHM_SIZE) == -1)
  {
    perror("ftruncate");
    exit(1);
  }

  char *shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED)
  {
    perror("mmap");
    exit(1);
  }

  int i = 0;
  while (pipe_commands[i] != NULL)
  {
    char *input = pipe_commands[i];

    if (pipe_commands[i + 1] != NULL) // Check if there's a next command
    {
      pipe(pipe_fds); // Create a pipe for the current command
      read_fd = pipe_fds[0];
    }
    else
    {
      read_fd = -1; // No more commands, set read_fd to -1
    }

    // Parse the user input into individual arguments
    char *args[MAX_LINE / 2 + 1];
    int background = 0;
    char *input_file = NULL;
    char *output_file = NULL;
    parse_input(input, args, &background, &input_file, &output_file);

    // Execute the current command with the shared memory
    execute_pipe_command(args, read_fd, pipe_fds[1]);

    // If there's a next command, close the write end of the current pipe
    if (pipe_commands[i + 1] != NULL)
    {
      close(pipe_fds[1]);
    }

    i++;
  }

  // Wait for all child processes to complete
  if (!background)
  {
    while (wait(NULL) > 0);
  }

  // Clean up shared memory
  if(munmap(shm_ptr, SHM_SIZE) == -1)
  {
    perror("munmap");
    exit(1);
  }

  if (shm_unlink(SHM_NAME) == -1)
  {
    perror("shm_unlink");
    exit(1);
  }
}

void execute_pipe_command(char **args, int read_fd, int write_fd)
{
  pid_t pid = fork(); // Fork a new process

  if (pid == 0) // Chile process
  {
    if (read_fd != -1) // Check if there's a pipe to read from
    {
      dup2(read_fd, STDIN_FILENO); // Redirect the standard input to read from the pipe
      close(read_fd);
    }
    else // If there's no pipe, read from the shared memory
    {
      int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
      if (shm_fd == -1)
      {
	perror("shm_open");
	exit(1);
      }

      char *shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
      if (shm_ptr == MAP_FAILED)
      {
	perror("mmap");
	exit(1);
      }
      dup2(shm_fd, STDIN_FILENO); // Redirect the standard input to read from theshared memory
      close(shm_fd);
    }
    
    if (write_fd != -1) // Check if there's a pipe to write to
    {
      dup2(write_fd, STDOUT_FILENO); // Redirect the standard output to write to the pipe
      close(write_fd);
    }
    else // If there's no pipe, write to the shared memory
    {
      int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
      if (shm_fd == -1)
      {
	perror("shm_open");
	exit(1);
      }

      char *shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
      if (shm_ptr == MAP_FAILED)
      {
	perror("mmap");
	exit(1);
      }
      dup2(shm_fd, STDOUT_FILENO); // Redirect the standatd output to write to the shared memory
      close(shm_fd);
    }
    
    execvp(args[0], args); // Execute the given command
    perror(args[0]);
    exit(1);
  }
  else if (pid < 0) // Fork failed
  {
    perror("fork");
    exit(1);
  }
}
