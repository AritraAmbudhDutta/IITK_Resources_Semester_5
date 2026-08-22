#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMANDS 16
#define MAX_ARG_LEN 1024

void parse_commands(char *input, char *commands[]) {
    int i = 0;
    char *token = strtok(input, "|");
    while (token != NULL && i < MAX_COMMANDS) {
        commands[i++] = token;
        token = strtok(NULL, "|");
    }
    commands[i] = NULL;  // Null-terminate the commands array
}

void execute_commands(char *commands[]) {
    int num_commands = 0;
    while (commands[num_commands] != NULL) num_commands++;

    int pipefd[2];
    int prev_pipefd[2] = {-1, -1};  // Store previous pipe fds

    for (int i = 0; i < num_commands; i++) {
        if (i < num_commands - 1) {
            // Not the last command, create a new pipe
            if (pipe(pipefd) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            if (prev_pipefd[0] != -1) {
                // Not the first command, set up stdin from previous pipe
                dup2(prev_pipefd[0], STDIN_FILENO);
                close(prev_pipefd[0]);
                close(prev_pipefd[1]);
            }

            if (i < num_commands - 1) {
                // Not the last command, set up stdout to current pipe
                close(pipefd[0]);  // Close read end of current pipe
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);  // Close write end after dup
            }

            // Execute the command
            char *args[MAX_ARG_LEN];
            int j = 0;
            char *arg = strtok(commands[i], " ");
            while (arg != NULL) {
                args[j++] = arg;
                arg = strtok(NULL, " ");
            }
            args[j] = NULL;

            execvp(args[0], args);
            // If execvp fails
            // perror("execvp");
            printf("Error");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            if (prev_pipefd[0] != -1) {
                close(prev_pipefd[0]);
                close(prev_pipefd[1]);
            }
            prev_pipefd[0] = pipefd[0];
            prev_pipefd[1] = pipefd[1];
        }
    }

    // Close last pipe if it's open
    if (prev_pipefd[0] != -1) {
        close(prev_pipefd[0]);
        close(prev_pipefd[1]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_commands; i++) {
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"command1 | command2 | ... | commandN\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *commands[MAX_COMMANDS + 1];
    parse_commands(argv[1], commands);
    execute_commands(commands);

    return 0;
}