/***********************************************************************
 *  File Name   : execute_internal_commands.c
 *  Description : Implements built-in shell commands.
 ***********************************************************************/

#include "main.h"

/* External shell state variables */
extern char cwd[256], prompt[512];
extern int ps1_flag, status;

void execute_internal_commands(char *input_string)
{
    /* Extract command */
    char *cmd = strtok(input_string, " ");

    if (strcmp(cmd, "cd") == 0)
    {
        /* Change working directory */
        char *path = strtok(NULL, "\n");

        if (path == NULL)
        {
            char *home = getenv("HOME");
            if (home == NULL || chdir(home) != 0)
                perror("cd");
        }
        else
        {
            if (chdir(path) != 0)
                perror("cd");
        }

        /* Update prompt if PS1 not modified */
        if (ps1_flag == 0)
        {
            getcwd(cwd, sizeof(cwd));
            char *home = getenv("HOME");
            if (home != NULL && strstr(cwd, home) == cwd)
                snprintf(cwd, sizeof(cwd), "~%s", cwd + strlen(home));

            snprintf(prompt, sizeof(prompt), "%s%s%s:%s%s%s$ ",
                     ANSI_COLOR_GREEN, PROMPT, ANSI_COLOR_RESET,
                     ANSI_COLOR_BLUE, cwd, ANSI_COLOR_RESET);
        }
    }
    else if (strcmp(cmd, "pwd") == 0)
    {
        /* Print current working directory */
        if (getcwd(cwd, sizeof(cwd)))
            printf("%s\n", cwd);
        else
            perror("pwd");
    }
    else if (strcmp(cmd, "echo") == 0)
    {
        /* Handle echo with shell variables */
        char *str = strtok(NULL, "\n");
        if (str[0] == '$')
        {
            if (str[1] == '?')
            {
                printf("%d\n", WEXITSTATUS(status));
                return;
            }
            else if (str[1] == '$')
            {
                printf("%d\n", getpid());
                return;
            }
            char *env_var = getenv(str + 1);
            if (env_var)
                printf("%s\n", env_var);
            else
                printf("\n");
            return;
        }
        printf("%s\n", str);
    }
    else if (strcmp(cmd, "printf") == 0)
    {
        /* Print formatted string */
        char *str = strtok(NULL, " ");
        printf("%s", str);
    }
    else if (strcmp(cmd, "exit") == 0)
    {
        /* Exit shell */
        exit(0);
    }
    else if (strcmp(cmd, "clear") == 0)
    {
        /* Clear terminal screen */
        printf(CLEAR_SCREEN);
    }
}
