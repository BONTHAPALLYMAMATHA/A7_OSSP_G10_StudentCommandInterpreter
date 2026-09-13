#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/shell.h"
#include "../include/input.h"
#include "../include/parser.h"
#include "../include/process.h"


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
    printf("  exit      - Exit the shell\n");
    printf("\n");
}


void execute_command(char *args[])
{
    if (args[0] == NULL)
    {
        return;
    }


    if (strcmp(args[0], "help") == 0)
    {
        show_help();
        return;
    }


    if (strcmp(args[0], "clear") == 0)
    {
        printf("\033[H\033[J");
        return;
    }


    if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == NULL)
        {
            printf("cd: missing directory\n");
        }
        else if (chdir(args[1]) != 0)
        {
            perror("cd");
        }

        return;
    }


    if (strcmp(args[0], "mkdir") == 0)
    {
        if (args[1] == NULL)
        {
            printf("mkdir: missing directory name\n");
        }
        else if (mkdir(args[1], 0755) != 0)
        {
            perror("mkdir");
        }

        return;
    }


    if (strcmp(args[0], "touch") == 0)
    {
        if (args[1] == NULL)
        {
            printf("touch: missing file name\n");
        }
        else
        {
            int fd = open(args[1], O_CREAT | O_WRONLY, 0644);

            if (fd == -1)
            {
                perror("touch");
            }
            else
            {
                close(fd);
            }
        }

        return;
    }


    if (strcmp(args[0], "cat") == 0)
    {
        if (args[1] == NULL)
        {
            printf("cat: missing file name\n");
        }
        else
        {
            int fd = open(args[1], O_RDONLY);

            if (fd == -1)
            {
                perror("cat");
            }
            else
            {
                char buffer[1024];
                ssize_t bytes_read;

                while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
                {
                    write(STDOUT_FILENO, buffer, bytes_read);
                }

                close(fd);
            }
        }

        return;
    }


    if (strcmp(args[0], "rm") == 0)
    {
        if (args[1] == NULL)
        {
            printf("rm: missing file name\n");
        }
        else if (unlink(args[1]) != 0)
        {
            perror("rm");
        }

        return;
    }


    /*
     * Week 4:
     * External commands are executed by process.c
     * using fork(), execvp(), and waitpid().
     */
    execute(args);
}


int main(void)
{
    char *line;


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


        if (strcmp(line, "exit") == 0)
        {
            free(line);
            break;
        }


        char **tokens;

        tokens = parse_line(line);


        execute_command(tokens);


        free_tokens(tokens);

        free(line);
    }


    return 0;
}
