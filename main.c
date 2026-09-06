/***********************************************************************
 *  File Name   : main.c
 *  Description : Entry point for the MiniShell application.
 *                Clears the screen and starts the shell input loop.
 ***********************************************************************/

#include "main.h"

/* Global prompt and input buffer declared in another source file */
extern char prompt[256], input_string[256];

int main()
{
    /* Clear terminal screen */
    printf(CLEAR_SCREEN);

    /* Start shell execution */
    scan_input(prompt, input_string);

    return 1;
}
