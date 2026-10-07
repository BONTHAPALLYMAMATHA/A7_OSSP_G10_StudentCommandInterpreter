#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../include/redirect.h"

int execute_redirection(char **args)
{
    int i;

    for (i = 0; args[i] != NULL; i++)
    {
        int fd;
        int target_fd;
        int flags;

        if (strcmp(args[i], ">") == 0)
        {
            if (args[i + 1] == NULL)
            {
                printf("Missing output file\n");
                return 1;
            }

            target_fd = STDOUT_FILENO;
            flags = O_WRONLY | O_CREAT | O_TRUNC;

            args[i] = NULL;

            fd = open(args[i + 1], flags, 0644);

            if (fd < 0)
            {
                perror("open");
                return 1;
            }

            pid_t pid = fork();

            if (pid < 0)
            {
                perror("fork");
                close(fd);
                return 1;
            }

            if (pid == 0)
            {
                if (dup2(fd, target_fd) == -1)
                {
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }

                close(fd);

                execvp(args[0], args);

                perror("execvp");
                exit(EXIT_FAILURE);
            }

            close(fd);
            waitpid(pid, NULL, 0);
            return 1;
        }

        if (strcmp(args[i], ">>") == 0)
        {
            if (args[i + 1] == NULL)
            {
                printf("Missing output file\n");
                return 1;
            }

            target_fd = STDOUT_FILENO;
            flags = O_WRONLY | O_CREAT | O_APPEND;

            args[i] = NULL;

            fd = open(args[i + 1], flags, 0644);

            if (fd < 0)
            {
                perror("open");
                return 1;
            }

            pid_t pid = fork();

            if (pid < 0)
            {
                perror("fork");
                close(fd);
                return 1;
            }

            if (pid == 0)
            {
                if (dup2(fd, target_fd) == -1)
                {
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }

                close(fd);

                execvp(args[0], args);

                perror("execvp");
                exit(EXIT_FAILURE);
            }

            close(fd);
            waitpid(pid, NULL, 0);
            return 1;
        }

        if (strcmp(args[i], "<") == 0)
        {
            if (args[i + 1] == NULL)
            {
                printf("Missing input file\n");
                return 1;
            }

            target_fd = STDIN_FILENO;
            flags = O_RDONLY;

            args[i] = NULL;

            fd = open(args[i + 1], flags);

            if (fd < 0)
            {
                perror("open");
                return 1;
            }

            pid_t pid = fork();

            if (pid < 0)
            {
                perror("fork");
                close(fd);
                return 1;
            }

            if (pid == 0)
            {
                if (dup2(fd, target_fd) == -1)
                {
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }

                close(fd);

                execvp(args[0], args);

                perror("execvp");
                exit(EXIT_FAILURE);
            }

            close(fd);
            waitpid(pid, NULL, 0);
            return 1;
        }

        if (strcmp(args[i], "2>") == 0)
        {
            if (args[i + 1] == NULL)
            {
                printf("Missing error output file\n");
                return 1;
            }

            target_fd = STDERR_FILENO;
            flags = O_WRONLY | O_CREAT | O_TRUNC;

            args[i] = NULL;

            fd = open(args[i + 1], flags, 0644);

            if (fd < 0)
            {
                perror("open");
                return 1;
            }

            pid_t pid = fork();

            if (pid < 0)
            {
                perror("fork");
                close(fd);
                return 1;
            }

            if (pid == 0)
            {
                if (dup2(fd, target_fd) == -1)
                {
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }

                close(fd);

                execvp(args[0], args);

                perror("execvp");
                exit(EXIT_FAILURE);
            }

            close(fd);
            waitpid(pid, NULL, 0);
            return 1;
        }
    }

    return 0;
}
