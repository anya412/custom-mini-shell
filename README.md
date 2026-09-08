# Custom Mini-Shell

Mini Shell is a lightweight Unix-like command-line interpreter implemented in C. It supports the execution of built-in and external commands, piping, signal handling, and a dynamic prompt that displays the current working directory.

## Features

*   **Custom shell prompt:** Displays `minishell:`
*   **Dynamic directory tracking:** Prompt updates to show the current working directory.
*   **Built-in commands:** Supports standard shell built-ins like `cd`, `pwd`, `echo`, and `exit`.
*   **External command execution:** Utilizes `fork()` and `execvp()` to run system commands.
*   **Pipe support:** Chain commands together using `|`.
*   **Signal handling:** Gracefully handles interrupts like `SIGINT` (Ctrl+C) and `SIGTSTP` (Ctrl+Z).
*   **Process synchronization:** Manages child processes using `wait()`.
*   **Robust parsing:** Includes input parsing and error handling for invalid commands.

## Technologies Used

*   C Programming
*   Linux System Calls
*   Process Management
*   Signals
*   File Descriptors and Pipes

## Project Structure

```text
minishell/
├── *.c
├── *.h
├── src/
│   ├── *.o
├── Makefile
├── README.md
└── .gitignore
```

## How the Prompt Works

The shell prompt is constructed by concatenating a static identifier (`minishell:`) with the current working directory. 

To optimize performance, the prompt is initialized before the main loop and updated only when the directory changes (i.e., after a successful `cd` command). This avoids unnecessary `getcwd()` calls on every iteration while keeping the prompt completely accurate.

## Getting Started

### How to Build

Use the provided Makefile to compile the project:

```bash
make
```

### How to Run

Execute the compiled binary to start the shell:

```bash
./minishell
```

## Sample Usage

```bash
minishell:/home/ananya$ pwd
/home/ananya
minishell:/home/ananya$ cd projects
minishell:/home/ananya/projects$ ls | grep shell
minishell:/home/ananya/projects$ echo Hello World
Hello World
```
