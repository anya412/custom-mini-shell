/***********************************************************************
 *  File Name   : shell_utils.c
 *  Description : Implements signal handling, command parsing, piping,
 *                and job control utilities for the mini shell.
 ***********************************************************************/

#include "main.h"

/* External shell state variables */
extern char prompt[256], input_string[256], cwd[256], *external_cmd[EXTERNAL_CMD_COUNT], *builtins_cmd[];
extern int pid, status, node_count;
extern Slist *head;

/* Handles SIGINT, SIGTSTP, and SIGCHLD signals */
void signal_handler(int sig_num)
{
    switch (sig_num)
    {
        case SIGINT:
            if (pid == 0)
                fprintf(stderr, "\n%s", prompt);
            break;

        case SIGTSTP:
            if (pid)
            {
                insert_at_first(&head, pid, input_string);
                node_count++;
                printf("[%d]+ Stopped\t\t%s\n", node_count, head->cmd);
            }
            break;

        case SIGCHLD:
        {
            int child_pid;
            while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0)
            {
                sl_delete_element(&head, child_pid);
                node_count--;
            }
        }
        break;
    }
}

/* Extracts command name from input string */
char *get_command(char *input_string)
{
    char *cmd = strdup(input_string);
    char *temp = strchr(cmd, ' ');
    if (temp)
        *temp = '\0';
    return cmd;
}

/* Loads external commands list from file */
void extract_external_commands(char **external_commands)
{
    FILE *fp = fopen("ext_cmd.txt", "r");
    char buffer[256];
    for (int i = 0; i < EXTERNAL_CMD_COUNT; i++)
    {
        fscanf(fp, "%s", buffer);
        external_cmd[i] = malloc(strlen(buffer) + 1);
        strcpy(external_cmd[i], buffer);
    }
}

/* Identifies whether command is built-in or external */
int check_command_type(char *command)
{
    for (int i = 0; builtins_cmd[i]; i++)
        if (strcmp(builtins_cmd[i], command) == 0)
            return BUILTIN;

    for (int i = 0; i < EXTERNAL_CMD_COUNT; i++)
        if (strcmp(external_cmd[i], command) == 0)
            return EXTERNAL;

    return NO_COMMAND;
}

/* Executes multiple commands connected via pipes */
void n_pipe(char *cmd[], int *cmd_pos, int cmd_count)
{
    int pip[2];

    for (int i = 0; i < cmd_count; i++)
    {
        if (i < cmd_count - 1)
        {
            if (pipe(pip) == -1)
            {
                perror("pipe");
                return;
            }
        }

        pid = fork();
        if (pid == 0)
        {
            signal(SIGINT, SIG_DFL);
            if (i < cmd_count - 1)
            {
                close(pip[0]);
                dup2(pip[1], 1);
                close(pip[1]);
            }
            execvp(cmd[cmd_pos[i]], cmd + cmd_pos[i]);
        }
        else if (pid > 0)
        {
            if (i < cmd_count - 1)
            {
                close(pip[1]);
                dup2(pip[0], 0);
                close(pip[0]);
            }
            wait(NULL);
        }
        else
        {
            perror("fork");
            return;
        }
    }

    int status;
    for (int i = 0; i < cmd_count; i++)
        waitpid(pid, &status, WUNTRACED);
}

/* Inserts a background job at the beginning of list */
int insert_at_first(Slist **head, int pid, char *cmd)
{
    Slist *newNode = malloc(sizeof(Slist));
    if (newNode == NULL)
        return 0;

    newNode->pid = pid;
    strcpy(newNode->cmd, cmd);
    newNode->link = *head;
    *head = newNode;
    return 1;
}

/* Deletes the first job from list */
int sl_delete_first(Slist **head)
{
    if (*head == NULL)
        return -1;

    Slist *temp = *head;
    *head = temp->link;
    free(temp);
    return 1;
}

/* Displays all stopped background jobs */
void print_list(Slist *head)
{
    if (head == NULL)
    {
        printf("-bash: %s: current: no such job\n", input_string);
    }
    else
    {
        for (int i = 1; head; i++)
        {
            printf("[%d]   Stopped\t\t%s\n", i, head->cmd);
            head = head->link;
        }
    }
}

/* Deletes a specific job from list using PID */
int sl_delete_element(Slist **head, int pid)
{
    if (*head == NULL)
        return -1;

    if ((*head)->pid == pid)
    {
        Slist *del = *head;
        *head = (*head)->link;
        free(del);
        return 1;
    }

    Slist *temp = *head;
    while (temp->link != NULL)
    {
        if (temp->link->pid == pid)
        {
            Slist *del = temp->link;
            temp->link = temp->link->link;
            free(del);
            return 1;
        }
        temp = temp->link;
    }
    return 0;
}
