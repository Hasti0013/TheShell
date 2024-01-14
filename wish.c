// Hasti Rathod
//3600 Major: The Shell
// To run, gcc wish.c -lreadline -o wish
// ./wish.c 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_SIZE 100
#define HISTORY_SIZE 100

char *history[HISTORY_SIZE];
int history_index = 0;

void parseInput(char *input, char **args) {
    int i = 0;
    char *token = strtok(input, " \n\t");
    while (token) {
        args[i] = token;
        token = strtok(NULL, " \n\t");
        i++;
    }
    args[i] = NULL;
}

void executeCommand(char **args) {
    if (args[0] == NULL) {
        return;
    }

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "wish: cd requires an argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("wish");
            }
        }
        return;
    }
   
    if (strcmp(args[0], "history") == 0) {
        for (int i = 0; i < history_index; i++) {
            printf("%d %s\n", i + 1, history[i]);
        }
        return;
    }
   
    if (strcmp(args[0], "exit") == 0) {
        for (int i = 0; i < history_index; i++) {
            free(history[i]);
        }
        exit(0);
    }

    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("wish");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("wish");
    } else {
        wait(NULL);
    }
}

int main(int argc, char **argv) {
    char *input;
    char *args[MAX_ARG_SIZE];

    // Check if the shell is being launched by the loader as an interpreter
    if (argc != 1) {
        int fd = open(argv[argc - 1], O_RDONLY);
        if (fd < 0) {
            perror(argv[argc - 1]);
            exit(1);
        }

        if (dup2(fd, STDIN_FILENO) < 0) {
            perror("dup2");
            exit(1);
        }
    }

    while (1) {
        input = readline("wish> ");
        if (input && *input) {
            add_history(input);
            if (history_index < HISTORY_SIZE) {
                history[history_index++] = strdup(input);
            } else {
                free(history[0]);
                for (int i = 1; i < HISTORY_SIZE; i++) {
                    history[i - 1] = history[i];
                }
                history[HISTORY_SIZE - 1] = strdup(input);
            }
        }

        parseInput(input, args);
        executeCommand(args);
        free(input);
    }

    return 0;
}
