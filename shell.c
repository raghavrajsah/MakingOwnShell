

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define PROMPT "shell> "
#define MAX_BACKGROUND_PROCESSES 50

char** tokenize(const char *input, const char *delimiters);
void free_tokens(char **tokens);
void fg(pid_t *back_pids, int *back_pids_ind, int *fg_pid);
void handle_finished_processes(pid_t *back_pids, int *back_pids_ind);
void execute_command(char **command, pid_t *back_pids, int *back_pids_ind, int *fg_pid);
void print_prompt();

int main(int argc, char **argv) {
    pid_t back_pids[MAX_BACKGROUND_PROCESSES];
    int back_pids_ind = 0;
    int fg_pid = -1;

    print_prompt();

    char buffer[1024];
    while (1) {
        buffer[0] = '\0';

        if (fgets(buffer, 1024, stdin) == NULL) {
            exit(EXIT_SUCCESS);
        }

        handle_finished_processes(back_pids, &back_pids_ind);

        char **command = tokenize(buffer, " \t\n");
        execute_command(command, back_pids, &back_pids_ind, &fg_pid);
        free_tokens(command);

        print_prompt();
    }

    return EXIT_SUCCESS;
}

// Function definitions

/*
 * Break input string into an array of strings.
 * @param input the string to tokenize
 * @param delimiters the characters that delimite tokens
 * @return the array of strings with the last element of the array set to NULL
 */
char** tokenize(const char *input, const char *delimiters) {
    char *token = NULL;

    // make a copy of the input string, because strtok
    // likes to mangle strings.  
    char *input_copy = strdup(input);

    // find out exactly how many tokens we have
    int count = 0;
    for (token = strtok(input_copy, delimiters); token; 
            token = strtok(NULL, delimiters)) {
        count++ ;
    }
    free(input_copy);

    input_copy = strdup(input);

    // allocate the array of char *'s, with one additional
    char **array = (char **)malloc(sizeof(char *)*(count+1));
    int i = 0;
    for (token = strtok(input_copy, delimiters); token;
            token = strtok(NULL, delimiters)) {
        array[i] = strdup(token); //where memory leak is occuring
        i++;
    }
    array[i] = NULL;
    free(input_copy);
    return array;
}

/*
 * Free all memory used to store an array of tokens.
 * @param tokens the array of tokens to free
 */
void free_tokens(char **tokens) {
    int i = 0;
    while (tokens[i] != NULL) {
        free(tokens[i]); // free each string
        i++;
    }
    free(tokens); // then free the array
}

//moves most recent background process to the foreground
void fg(pid_t *back_pids, int *back_pids_ind, int *fg_pid) {
    if (*back_pids_ind == 0) {
        printf("No background processes to move to the foreground.\n");
        return;
    }

    pid_t pid = back_pids[*back_pids_ind - 1];
    (*back_pids_ind)--;
    *fg_pid = pid;

    int status;
    if(waitpid(pid, &status, 0) < 0){
        printf("Process failed\n");
    }; //moves most recent background process to foreground
    *fg_pid = -1; // Reset fg_pid after the process is moved to the foreground
}

//checks if a background has finished, and if so prints a message
void handle_finished_processes(pid_t *back_pids, int *back_pids_ind) {
    int status;
    pid_t finished_pid;
    while ((finished_pid = waitpid(-1, &status, WNOHANG)) > 0) { //if process has finished, print message and reap
        printf("Process %d finished\n", finished_pid);
        for (int i = 0; i < *back_pids_ind; i++) {
            if (back_pids[i] == finished_pid) { //remove pid from array of background pids
                for (int j = i + 1; j < *back_pids_ind; j++) {
                    back_pids[j - 1] = back_pids[j];
                }
                (*back_pids_ind)--;
                break;
            }
        }
    }
}

//executes shell prompt by forking and creating a child process
void execute_command(char **command, pid_t *back_pids, int *back_pids_ind, int *fg_pid) {
    int len = 0;
    while (command[len] != NULL) {
        len++;
    }

    int background = 0;
    if (len > 0 && strcmp(command[len - 1], "&") == 0) { //eliminates & from the command arguements
        background = 1;
        free(command[len]);
        command[len - 1] = NULL; //this is causing the memory error
    }

    if (command[0] == NULL) {
        // Do nothing
    } else if (strcmp(command[0], "fg") == 0) {
        fg(back_pids, back_pids_ind, fg_pid);
    } else {
        pid_t pid = fork();
        if (pid == 0) { //if child process, execute
            if (execvp(command[0], command) < 0) {
                perror("command not found");
                exit(EXIT_FAILURE);
            }
        } else if (pid > 0) { //if parent, wait for process or add to background processes
            if (!background) {
                // Foreground process
                if(waitpid(pid, NULL, 0) < 0){
                    printf("Process failed\n");
                };
            } else {
                // Background process
                if (*back_pids_ind < MAX_BACKGROUND_PROCESSES) {
                    back_pids[(*back_pids_ind)++] = pid;
                    printf("Process %d started\n", pid);
                } else {
                    printf("Maximum number of background processes reached.\n");
                }
            }
        } else {
            printf("Fork failed\n");
        }
    }
}

//prints out the shell prompt
void print_prompt() {
    if (isatty(0)) {
        printf("%s", PROMPT);
        fflush(stdout);
    }
}
