#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80
#define HISTORY_SIZE 10

void parse_input(char *input, char **args, int *background);
// Parses the user input into individual arguments and determines if the command should be run in the background.

void add_to_history(char *input, char **history, int *history_count);
// Adds the user input to the command history array.

void execute_previous_command(char **args, char **history, int *history_count);
// Executes the previous command from the command history.

void execute_command(char **args, int background);
// Executes the given command with the specified arguments.

int main(void)
{
    char *args[MAX_LINE / 2 + 1];
    int should_run = 1;
    int background;
    char *history[HISTORY_SIZE];
    int history_count = 0;

    while (should_run)
    {
        printf("ashell>"); // Prompt the user for input
        fflush(stdout);

        char input[MAX_LINE + 1];
        fgets(input, sizeof(input), stdin); // Read user input

        if (strcmp(input, "!!\n") == 0) // Check if user input is '!!' to execute previous command
        {
            execute_previous_command(args, history, &history_count);
            continue;
        }

        add_to_history(input, history, &history_count); // Add user input to command history
        parse_input(input, args, &background); // Parse the user input into individual arguments

        if (strcmp(args[0], "exit") == 0) // Check if user wants to exit the shell
        {
            should_run = 0;
            continue;
        }
        else if ((strcmp(args[0], "") == 0) && args[1] == NULL) // Check if user entered an empty command
        {
            continue;
        }

        execute_command(args, background); // Execute the user command
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
        if (strcmp(token, "&") == 0) // Check if the argument is '&', indicating background execution
        {
            *background = 1;
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

void execute_previous_command(char **args, char **history, int *history_count)
{
    if (*history_count > 0) // Check if there are commands in the history
    {
        (*history_count)--; // Decrement the history count to get the index of the previous command
        char *previous_command = history[*history_count]; // Get the previous command from the history
        printf("Executing previous command: %s", previous_command);
        int background = 0;
        memset(args, 0, sizeof(args)); // Clear the arguments array
        parse_input(previous_command, args, &background); // Parse the previous command into arguments
        execute_command(args, background); // Execute the previous command
        free(previous_command); // Free the memory of the previous command
    }
    else
    {
        printf("No previous command in history.\n");
    }
}

void execute_command(char **args, int background)
{
    pid_t pid = fork(); // Fork a new process

    if (pid == 0) // Child process
    {
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

