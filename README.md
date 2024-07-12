# Making Own Shell(CLI) in C

## Overview
This project involves creating a simple shell, which operates by creating child processes to execute commands entered by the user. The shell functions similarly to the `bash` shell but includes several simplifications to make it more manageable.

## Features

### Prompt and Input
- **Interactive Prompt**: The shell displays a prompt (`shell>`) and waits for user input.
- **Command Input**: Users can enter commands with the full path to the executable (e.g., `/bin/ls`) along with command-line options.

### Command Execution
- **Non-built-in Commands**: When a user enters a command, the shell creates a new process to run the specified program.
- **Foreground Execution**: Commands without an ampersand (`&`) run in the foreground, and the shell waits for them to complete.
- **Background Execution**: Commands ending with an ampersand (`&`) run in the background, allowing the shell to prompt for new commands immediately. The shell also outputs the PID of the background process.

### Process Management
- **Graceful Error Handling**: The shell handles cases where the specified executable cannot be invoked.
- **Background Process Completion**: The shell checks for completed background processes and outputs their PIDs, ensuring zombie processes are reaped using `waitpid` with `WNOHANG`.
- **Moving Background Commands to Foreground**: The `fg` command moves the most recent background process to the foreground. If this process has already finished, the next most recent background process is moved to the foreground.

## Getting Started
1. Clone your repository on your Raspberry Pi (RPi).
2. Run your shell interactively by executing the compiled program.

### Main Function
The main function in `main.c` includes:
1. Displaying the prompt (`shell>`)
2. Reading user input into a buffer
3. Tokenizing the input command, where tokens are delimited by whitespace

## Sample Commands
Here are some example commands to test the shell:

```sh
shell> /usr/bin/ls
shell> /usr/bin/ls -a
shell> /usr/bin/date
shell> /usr/bin/sleep 5
shell> /usr/bin/sleep 10 &
Process 5328 started
shell> /usr/bin/sleep 5 &
Process 5329 started
shell> /usr/bin/ps
shell> /usr/bin/sleep 15
shell> /usr/bin/ps
Process 5329 finished
Process 5328 finished
shell> /usr/bin/sleep 7 &
Process 5333 started
shell> fg
shell> /usr/bin/echo Hello
Hello
```

### Notes
- **Foreground Execution**: The shell waits for commands like `/usr/bin/sleep 5` to complete before displaying the prompt again.
- **Background Execution**: Commands like `/usr/bin/sleep 10 &` run in the background, and the prompt appears immediately.
- **Process Listing**: Use `/usr/bin/ps` to list processes and verify background processes.
- **Foreground Command**: The `fg` command moves the most recent background process to the foreground.


