/***********************************************************************
 *  File Name   : main.h
 *  Description : Header file for the MiniShell project.
 *                Contains macro definitions, data structures,
 *                function prototypes, and required system headers
 *                used across the MiniShell application.
 *
 *                This file supports:
 *                - Command parsing and classification
 *                - Execution of built-in and external commands
 *                - Signal handling (SIGINT, SIGTSTP, SIGCHLD)
 *                - Process management and job control
 *                - ANSI color formatting for prompt display
 *                - Linked list based background process tracking
 *
 *                Major Components:
 *                - Macro definitions for command types
 *                - ANSI escape sequences for colored output
 *                - Singly linked list for job control
 *                - Function declarations for shell operations
 ***********************************************************************/

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define BUILTIN         1   /* Built-in shell command */
#define EXTERNAL        2   /* External system command */
#define NO_COMMAND      3   /* Invalid or empty command */

/* Maximum number of external commands supported */
#define EXTERNAL_CMD_COUNT 152

/* ANSI Color Definitions */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/* Clear terminal screen */
#define CLEAR_SCREEN       "\x1b[H\x1b[2J"

/* Default shell prompt name */
#define PROMPT "minishell"

/*
 * Structure : Slist
 * Purpose   : Stores background process information
 * Members   :
 *   pid  - Process ID of background job
 *   cmd  - Command associated with the process
 *   link - Pointer to next node
 */
typedef struct node
{
    int pid;
    char cmd[256];
    struct node *link;
} Slist;

/* Reads user input and displays shell prompt */
void scan_input(char *prompt, char *input_string);

/* Extracts the command name from input string */
char *get_command(char *input_string);

/* Handles prompt modification (e.g., PS1 changes) */
void copy_change(char *prompt, char *input_string);

/* Determines command type: BUILTIN / EXTERNAL / NO_COMMAND */
int check_command_type(char *command);

/* Implements echo command with special shell variables */
void echo(char *input_string, int status);

/* Executes built-in shell commands */
void execute_internal_commands(char *input_string);

/* Executes external commands using fork & exec */
void execute_external_commands(char *input_string);

/* Signal handler for SIGINT, SIGTSTP, SIGCHLD */
void signal_handler(int sig_num);

/* Extracts list of supported external commands */
void extract_external_commands(char **external_commands);

/* Executes commands connected via n pipes */
void n_pipe(char *cmd[], int *cmd_pos, int cmd_count);

/* Linked List Operations */
/* Inserts a new background process at beginning of list */
int insert_at_first(Slist **head, int data, char *cmd);

/* Deletes the first node of the list */
int sl_delete_first(Slist **head);

/* Prints all background jobs */
void print_list(Slist *head);

/* Deletes a specific process node using PID */
int sl_delete_element(Slist **head, int pid);

#endif
