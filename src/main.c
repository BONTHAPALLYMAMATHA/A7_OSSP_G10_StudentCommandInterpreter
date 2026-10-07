#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/shell.h"
#include "../include/input.h"
#include "../include/parser.h"
#include "../include/process.h"
#include "../include/builtin.h"
#include "../include/signals.h"
#include "../include/pipes.h"


void display_prompt(void)
{
    printf("Student Shell > ");
    fflush(stdout);
}


void execute_command(char *args[])
{
    if (args[0] == NULL)
    {
        return;
    }

    if (execute_builtin(args) == 0)
    {
        execute(args);
    }
}


void tokenize_command(char *str, char **argv)
{
    int i = 0;

    char *token = strtok(str, " \t\r\n");

    while (token != NULL)
    {
        argv[i++] = token;
        token = strtok(NULL, " \t\r\n");
    }

    argv[i] = NULL;
}


int main(void)
{
    char *line;
    char **tokens;

    initialize_signals();

    while (1)
    {
        display_prompt();

        line = read_line();

        if (line == NULL)
        {
            printf("\n");
            break;
        }

        if (strlen(line) == 0)
        {
            free(line);
            continue;
        }

        /*
         * Check whether the command contains a pipe.
         */
        if (strchr(line, '|') != NULL)
        {
            char *argv1[64];
            char *argv2[64];

            char *left = strtok(line, "|");
            char *right = strtok(NULL, "|");

            if (left == NULL || right == NULL)
            {
                printf("Invalid pipe command\n");
                free(line);
                continue;
            }

            tokenize_command(left, argv1);
            tokenize_command(right, argv2);

            if (argv1[0] == NULL || argv2[0] == NULL)
            {
                printf("Invalid pipe command\n");
                free(line);
                continue;
            }

            execute_pipe(argv1, argv2);

            free(line);
            continue;
        }

        /*
         * Normal command without pipe.
         */
        tokens = parse_line(line);

        execute_command(tokens);

        free_tokens(tokens);
        free(line);
    }

    return 0;
}
