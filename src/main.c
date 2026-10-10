#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "input.h"
#include "parser.h"
#include "builtin.h"
#include "signals.h"
#include "pipes.h"
#include "redirect.h"
#include "thread.h"
#include "job_control.h"

void display_prompt(void)
{
    printf("Student Shell > ");
    fflush(stdout);
}

static int has_redirection(const char *command)
{
    return strchr(command, '>') != NULL ||
           strchr(command, '<') != NULL;
}

static void run_shell_command(char **args, const char *command, int background)
{
    if (args[0] == NULL)
        return;

    if (execute_builtin(args))
        return;

    if (background)
    {
        if (has_redirection(command))
        {
            fprintf(stderr,
                    "Background redirection is not supported yet.\n");
            return;
        }

        launch_job(args, command, 1);
        return;
    }

    if (execute_redirection(args) == 0)
        launch_job(args, command, 0);
}

static void tokenize_command(char *str, char **argv)
{
    int i = 0;
    char *token = strtok(str, " \t\r\n");

    while (token != NULL && i < 63)
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
    init_job_control();

    run_thread_demo();
    start_monitor_thread();

    while (1)
    {
        char command[256];

        display_prompt();
        line = read_line();

        if (line == NULL)
        {
            printf("\n");
            break;
        }

        if (strspn(line, " \t\r\n") == strlen(line))
        {
            free(line);
            continue;
        }

        /*
         * Preserve the original command before tokenization
         * changes the input buffer.
         */
        snprintf(command, sizeof(command), "%s", line);

        if (strchr(line, '|') != NULL)
        {
            char *argv1[64];
            char *argv2[64];

            char *left = strtok(line, "|");
            char *right = strtok(NULL, "|");

            if (left == NULL || right == NULL ||
                strchr(right, '|') != NULL)
            {
                fprintf(stderr, "Invalid pipe command\n");
                free(line);
                continue;
            }

            tokenize_command(left, argv1);
            tokenize_command(right, argv2);

            if (argv1[0] == NULL || argv2[0] == NULL)
            {
                fprintf(stderr, "Invalid pipe command\n");
                free(line);
                continue;
            }

            execute_pipe(argv1, argv2);
            free(line);
            continue;
        }

        tokens = parse_line(line);

        if (tokens == NULL)
        {
            free(line);
            continue;
        }

        int count = 0;
        while (tokens[count] != NULL)
            count++;

        int background = 0;

        if (count > 0 && strcmp(tokens[count - 1], "&") == 0)
        {
            tokens[count - 1] = NULL;
            background = 1;
        }

        run_shell_command(tokens, command, background);

        free_tokens(tokens);
        free(line);
    }

    return 0;
}
