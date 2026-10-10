#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jobs.h"
#include "job_control.h"

static int parse_job_id(const char *arg)
{
    if (arg == NULL)
        return -1;

    if (*arg == '%')
        arg++;

    char *end;
    long id = strtol(arg, &end, 10);

    if (*arg == '\0' || *end != '\0' || id < 1 || id > MAX_JOBS)
        return -1;

    return (int)id;
}

int execute_builtin(char **args)
{
    char cwd[1024];

    if (args[0] == NULL)
        return 1;

    if (strcmp(args[0], "exit") == 0)
        exit(EXIT_SUCCESS);

    if (strcmp(args[0], "pwd") == 0)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf("%s\n", cwd);
        else
            perror("pwd");
        return 1;
    }

    if (strcmp(args[0], "cd") == 0)
    {
        const char *path = args[1] ? args[1] : getenv("HOME");

        if (path == NULL)
            fprintf(stderr, "cd: HOME is not set\n");
        else if (chdir(path) != 0)
            perror("cd");

        return 1;
    }

    if (strcmp(args[0], "clear") == 0)
    {
        system("clear");
        return 1;
    }

    if (strcmp(args[0], "help") == 0)
    {
        printf("\nShellForge Built-in Commands\n");
        printf("----------------------------\n");
        printf("cd [directory]  Change directory\n");
        printf("pwd             Show current directory\n");
        printf("clear           Clear terminal\n");
        printf("env             Show environment variables\n");
        printf("jobs            List background/stopped jobs\n");
        printf("fg %%1           Resume job 1 in foreground\n");
        printf("bg %%1           Resume stopped job 1 in background\n");
        printf("help            Show this help\n");
        printf("exit            Exit ShellForge\n");
        printf("Use command & to run a command in the background.\n\n");
        return 1;
    }

    if (strcmp(args[0], "env") == 0)
    {
        printf("HOME = %s\n", getenv("HOME") ? getenv("HOME") : "");
        printf("USER = %s\n", getenv("USER") ? getenv("USER") : "");
        printf("PATH = %s\n", getenv("PATH") ? getenv("PATH") : "");
        return 1;
    }

    if (strcmp(args[0], "jobs") == 0)
    {
        list_jobs();
        return 1;
    }

    if (strcmp(args[0], "fg") == 0 || strcmp(args[0], "bg") == 0)
    {
        int id = parse_job_id(args[1]);

        if (id < 1)
        {
            printf("Usage: %s %%job_id\n", args[0]);
            return 1;
        }

        if (strcmp(args[0], "fg") == 0)
            bring_job_foreground(id);
        else
            continue_job_background(id);

        return 1;
    }

    return 0;
}
