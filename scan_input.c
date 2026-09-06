/***********************************************************************
 *  File Name   : scan_input.c
 *  Description : Handles shell prompt display, user input parsing,
 *                command execution, and job control.
 ***********************************************************************/

#include "main.h"

/* List of supported built-in shell commands */
char *builtins_cmd[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
                        "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
                        "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "clear", NULL};

/* Global buffers and command storage */
char prompt[512], input_string[256], cwd[256], *external_cmd[EXTERNAL_CMD_COUNT];

/* Process and shell state variables */
int pid, status, node_count, ps1_flag;

/* Head pointer for background job list */
Slist *head;

void scan_input(char *prompt, char *input_string)
{
    /* Register signal handlers */
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler);

    /* Initialize input buffer */
    input_string[0] = '\0';

    /* Load external commands list */
    extract_external_commands(external_cmd);

    /* Build default shell prompt */
    char temp[512];
    getcwd(cwd, sizeof(cwd));
    snprintf(temp, sizeof(temp), "%s%s%s:%s%s%s$ ", ANSI_COLOR_GREEN, PROMPT,
             ANSI_COLOR_RESET, ANSI_COLOR_BLUE, cwd, ANSI_COLOR_RESET);
    strcpy(prompt, temp);

    while (1)
    {
        /* Clear input buffer */
        bzero(input_string, 256);

        /* Display prompt */
        printf("%s", prompt);
        fflush(stdout);

        /* Read user input */
        scanf("%[^\n]", input_string);
        __fpurge(stdin);

        /* Ignore empty input */
        if (input_string[0] == '\0')
            continue;

        /* Handle PS1 prompt change */
        if (strncmp(input_string, "PS1=", 4) == 0)
        {
            char temp[256];
            strcpy(temp, input_string);

            char *token = strtok(temp, " ");
            if (isalpha(input_string[4]))
            {
                strcpy(prompt, token + 4);
                ps1_flag = 1;
            }

            token = strtok(NULL, "\n");
            if (token)
                strcpy(input_string, token);
            else
                continue;
        }

        /* Extract command */
        char *cmd = get_command(input_string);

        /* Identify command type */
        int type = check_command_type(cmd);

        switch (type)
        {
            case BUILTIN:
                execute_internal_commands(input_string);
                break;

            case EXTERNAL:
                execute_external_commands(input_string);
                break;

            case NO_COMMAND:
                if (strcmp(input_string, "jobs") == 0)
                {
                    print_list(head);
                }
                else if (strcmp(input_string, "fg") == 0)
                {
                    if (head == NULL)
                    {
                        printf("-bash: fg: current: no such job\n");
                        continue;
                    }
                    printf("%s\n", head->cmd);
                    kill(head->pid, SIGCONT);
                    waitpid(head->pid, &status, WUNTRACED);
                    if (WIFEXITED(status) || WIFSIGNALED(status))
                    {
                        printf("[%d]+ Done\t\t%s\n", node_count, head->cmd);
                        sl_delete_first(&head);
                        node_count--;
                    }
                }
                else if (strcmp(input_string, "bg") == 0)
                {
                    if (head == NULL)
                    {
                        printf("-bash: fg: current: no such job\n");
                        continue;
                    }
                    printf("[%d]+ %s &\n", node_count, head->cmd);
                    kill(head->pid, SIGCONT);
                    sl_delete_first(&head);
                    node_count--;
                }
                else
                {
                    printf("%s: command not found\n", input_string);
                }
                break;
        }

        /* Reset input buffer */
        input_string[0] = '\0';
    }
}
