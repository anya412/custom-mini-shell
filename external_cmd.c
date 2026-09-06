/***********************************************************************
 *  File Name   : execute_external_commands.c
 *  Description : Executes external shell commands with support for pipes.
 ***********************************************************************/

#include "main.h"

/* Global process status and PID */
extern int status, pid;

void execute_external_commands(char *input_string)
{
    /* Ignore null input */
    if (input_string == NULL)
        return;

    /* Tokenize input command */
    char temp[256], *cmd[50];
    strcpy(temp, input_string);

    int i = 0;
    char *token = strtok(temp, " ");
    while (token != NULL)
    {
        cmd[i++] = token;
        token = strtok(NULL, " ");
    }
    cmd[i] = NULL;

    /* Identify pipe positions */
    int token_count = i;
    int cmd_count = 1, cmd_pos[token_count], j = 0;
    cmd_pos[0] = 0;

    while (j < token_count)
    {
        if (cmd[j][0] == '|')
        {
            cmd_pos[cmd_count++] = j + 1;
            cmd[j] = NULL;
        }
        j++;
    }

    /* Create child process */
    pid = fork();
    if (pid == 0)
    {
        /* Restore default signal handling */
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        /* Execute piped commands */
        if (cmd_count > 1)
        {
            n_pipe(cmd, cmd_pos, cmd_count);
            exit(0);
        }

        /* Execute single external command */
        execvp(cmd[0], cmd);
        perror("execvp failed");
        exit(0);
    }
    else if (pid > 0)
    {
        /* Wait for child process */
        waitpid(pid, &status, WUNTRACED);
        pid = 0;
    }
}
