#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/shell.h"
#include "../include/input.h"
#include "../include/parser.h"
#include "../include/process.h"
#include "../include/builtin.h"
#include "../include/signals.h"


void display_prompt(void)
{
    printf("Student Shell > ");
    fflush(stdout);
}


void show_help(void)
{
    printf("\n");
    printf("Student Command Interpreter\n");
    printf("---------------------------\n");
    printf("Available commands:\n");
    printf("  pwd       - Show current directory\n");
    printf("  ls        - List files and directories\n");
    printf("  mkdir     - Create a directory\n");
    printf("  touch     - Create a file\n");
    printf("  cat       - Display file contents\n");
    printf("  rm        - Remove a file\n");
    printf("  cd        - Change directory\n");
    printf("  clear     - Clear the screen\n");
    printf("  help      - Show this help message\n");
    printf("  env       - Show environment variables\n");
    printf("  exit      - Exit the shell\n");
    printf("\n");
}


void execute_command(char *args[])
{
    if (args[0] == NULL)
    {
        return;
    }

    /*
     * Check whether the command is a built-in command.
     */
    if (execute_builtin(args) == 0)
    {
        /*
         * If it is not built-in, execute it as
         * an external Linux command.
         */
        execute(args);
    }
}


int main(void)
{
    char *line;
    char **tokens;

    /*
     * Initialize signal handlers before
     * entering the shell loop.
     */
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

        tokens = parse_line(line);

        execute_command(tokens);

        free_tokens(tokens);
        free(line);
    }

    return 0;
}
