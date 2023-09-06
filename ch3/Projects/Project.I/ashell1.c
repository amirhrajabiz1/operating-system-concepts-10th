#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80
#define HISTORY_SIZE 10

void parse_input(char *input, char **args, int *background);
void add_to_history(char *input, char **history, int *history_count);
void execute_previous_command(char **args, char **history, int *history_count);
void execute_command(char **args, int background);

int main(void)
{
  char *args[MAX_LINE / 2 + 1];
  int should_run = 1;
  int background;
  char *history[HISTORY_SIZE]; // Store up to 10 previous commands
  int history_count = 0;

  while (should_run)
  {
    printf("ashell>");
    fflush(stdout);

    char input[MAX_LINE + 1];
    fgets(input, sizeof(input) , stdin);

    if (strcmp(input, "!!\n") == 0) // Execute previous command
    {
      execute_previous_command(args, history, &history_count);
      continue;
    }

    add_to_history(input, history, &history_count);
    parse_input(input, args, &background);


    if (strcmp(args[0], "exit") == 0)
    {
      should_run = 0;
      continue;
    } 
    else if ((strcmp(args[0], "") == 0) && args[1] == NULL)
    {
      continue;
    }

    execute_command(args, background);
  }

  return 0;
}


void parse_input(char *input, char **args, int *background)
{
  int i = 0;
  char *token = strtok(input, " \n");
  *background = 0;

  while (token != NULL)
  {
    if (strcmp(token, "&") == 0)
    {
      *background = 1;
    }
    else
    {
      args[i++] = token;
    }

    token = strtok(NULL, " \n");
  }

  args[i] = NULL;

  if (i == 0)
  {
    args[0] = "";
    args[1] = NULL;
  }

}


void add_to_history(char *input, char **history, int *history_count)
{
  if (*history_count < HISTORY_SIZE)
  {
    history[*history_count] = strdup(input);
    (*history_count)++;
  }
  else
  {
    free(history[0]);
    for (int i = 0; i < HISTORY_SIZE - 1; i++)
    {
      history[i] = history[i + 1];
    }
    history[HISTORY_SIZE - 1] = strdup(input);
  }
}


void execute_previous_command(char **args, char **history, int *history_count)
{
  if (*history_count > 0)
  {
    (*history_count)--;
    char *previous_command = history[*history_count];
    printf("Executing previous command: %s", previous_command);
    int background = 0;
    memset(args, 0, sizeof(args));
    parse_input(previous_command, args, &background);

    pid_t pid = fork();
    if (pid == 0)
    {
      execvp(args[0], args);
      perror("execvp");
      exit(1);
    }
    else if (pid > 0)
    {
      wait(NULL);
    }
    else
    {
      perror("fork");
      exit(1);
    }
   
    free(previous_command);
  }
  else
  {
    printf("No previous command in history.\n");
  }
}

void execute_command(char **args, int background)
{
  pid_t pid = fork();

  if (pid == 0)
  {
    execvp(args[0], args);
    perror("execvp");
    exit(1);
  }
  else if (pid > 0)
  {
    if (!background)
    {
      wait(NULL);
    }
  }
  else
  {
    perror("fork");
    exit(1);
  }
}
