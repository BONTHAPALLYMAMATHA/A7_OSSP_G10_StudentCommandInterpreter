#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/shell.h"
#include "../include/input.h"


void display_prompt(void)
{
    printf("Student Shell > ");
    fflush(stdout);
}


void parse_command(char *input, char *args[])
{
    int i = 0;

    char *token = strtok(input, " ");

    while (token != NULL && i < MAX_ARGS - 1)
    {
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    args[i] = NULL;
}


void show_help(void)
{
    printf("\n");
    printf("Student Command Interpreter\n");
    printf("---------------------------\n");
    printf("pwd              Show current directory\n");
    printf("ls               List files\n");
    printf("mkdir <name>     Create directory\n");
    printf("touch <name>     Create file\n");
    printf("cat <file>       Display file\n");
    printf("rm <file>        Delete file\n");
    printf("cd <directory>   Change directory\n");
    printf("clear            Clear screen\n");
    printf("help             Show help\n");
    printf("exit             Exit shell\n");
    printf("date             Show date\n");
    printf("whoami           Show current user\n");
    printf("uname            Show system information\n");
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
        system("clear");
        return;
    }


    if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == NULL)
        {
            printf("Usage: cd <directory>\n");
            return;
        }

        if (chdir(args[1]) == -1)
        {
            perror("cd");
        }

        return;
    }


    if (strcmp(args[0], "mkdir") == 0)
    {
        if (args[1] == NULL)
        {
            printf("Usage: mkdir <directory_name>\n");
            return;
        }

        if (mkdir(args[1], 0755) == -1)
        {
            perror("mkdir");
        }
        else
        {
            printf("Directory '%s' created successfully.\n", args[1]);
        }

        return;
    }


    if (strcmp(args[0], "touch") == 0)
    {
        if (args[1] == NULL)
        {
            printf("Usage: touch <file_name>\n");
            return;
        }

        int fd = open(args[1], O_CREAT | O_WRONLY, 0644);

        if (fd == -1)
        {
            perror("touch");
        }
        else
        {
            close(fd);
            printf("File '%s' created successfully.\n", args[1]);
        }

        return;
    }


    if (strcmp(args[0], "cat") == 0)
    {
        if (args[1] == NULL)
        {
            printf("Usage: cat <file_name>\n");
            return;
        }

        FILE *file = fopen(args[1], "r");

        if (file == NULL)
        {
            perror("cat");
            return;
        }

        char ch;

        while ((ch = fgetc(file)) != EOF)
        {
            putchar(ch);
        }

        fclose(file);

        return;
    }


    if (strcmp(args[0], "rm") == 0)
    {
        if (args[1] == NULL)
        {
            printf("Usage: rm <file_name>\n");
            return;
        }

        if (remove(args[1]) == -1)
        {
            perror("rm");
        }
        else
        {
            printf("File '%s' deleted successfully.\n", args[1]);
        }

        return;
    }


    /* Execute normal Linux commands */

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }


    if (pid == 0)
    {
        execvp(args[0], args);

        perror("Command execution failed");
        exit(EXIT_FAILURE);
    }


    waitpid(pid, NULL, 0);
}


int main(void)
{
    char *line;

    printf("\n");
    printf("========================================\n");
    printf("   Student Command Interpreter v2.0\n");
    printf("========================================\n");
    printf("Type 'help' for available commands.\n\n");


    while (1)
    {
        display_prompt();

        line = read_line();

        if (line == NULL)
        {
            printf("\n");
            break;
        }


        if (strcmp(line, "exit") == 0)
        {
            free(line);
            break;
        }


        if (strlen(line) == 0)
        {
            free(line);
            continue;
        }


        char *args[MAX_ARGS];

        parse_command(line, args);

        execute_command(args);

        free(line);
    }


    printf("Goodbye!\n");

    return 0;
}
