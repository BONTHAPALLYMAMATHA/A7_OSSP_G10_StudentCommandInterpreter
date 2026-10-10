#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

#include "jobs.h"
#include "job_control.h"

static pid_t shell_pgid;
static struct termios shell_tmodes;
static int interactive;

void init_job_control(void)
{
    interactive = isatty(STDIN_FILENO);
    shell_pgid = getpid();

    if (interactive)
    {
        while (tcgetpgrp(STDIN_FILENO) != getpgrp())
            kill(-getpgrp(), SIGTTIN);

        if (setpgid(shell_pgid, shell_pgid) < 0 &&
            errno != EACCES && errno != EPERM)
            perror("setpgid");

        shell_pgid = getpgrp();

        if (tcsetpgrp(STDIN_FILENO, shell_pgid) < 0)
            perror("tcsetpgrp");

        if (tcgetattr(STDIN_FILENO, &shell_tmodes) < 0)
            perror("tcgetattr");
    }

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, SIG_DFL);

    init_jobs();
}

static void restore_shell_terminal(void)
{
    if (!interactive)
        return;

    tcsetpgrp(STDIN_FILENO, shell_pgid);
    tcsetattr(STDIN_FILENO, TCSADRAIN, &shell_tmodes);
}

static void wait_for_foreground(pid_t pgid, const char *command)
{
    int status;
    pid_t result;

    do
    {
        result = waitpid(-pgid, &status, WUNTRACED);
    }
    while (result < 0 && errno == EINTR);

    restore_shell_terminal();

    if (result < 0)
    {
        if (errno != ECHILD)
            perror("waitpid");
        return;
    }

    if (WIFSTOPPED(status))
    {
        int id = add_job(pgid, command, JOB_STOPPED);
        if (id > 0)
            printf("\n[%d]+ Stopped %s\n", id, command);
    }
}

int launch_job(char **argv, const char *command, int background)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        setpgid(0, 0);

        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        if (!background && interactive)
            tcsetpgrp(STDIN_FILENO, getpid());

        execvp(argv[0], argv);
        perror(argv[0]);
        _exit(127);
    }

    if (setpgid(pid, pid) < 0 && errno != EACCES && errno != ESRCH)
        perror("setpgid");

    if (background)
    {
        int id = add_job(pid, command, JOB_RUNNING);
        if (id > 0)
            printf("[%d] %d\n", id, (int)pid);
        return 0;
    }

    if (interactive && tcsetpgrp(STDIN_FILENO, pid) < 0)
        perror("tcsetpgrp");

    wait_for_foreground(pid, command);
    return 0;
}

void continue_job_background(int job_id)
{
    Job *job = get_job(job_id);

    if (job == NULL)
    {
        printf("No such job: %d\n", job_id);
        return;
    }

    if (job->state != JOB_STOPPED)
    {
        printf("Job %d is not stopped.\n", job_id);
        return;
    }

    if (kill(-job->pgid, SIGCONT) < 0)
    {
        perror("bg");
        return;
    }

    job->state = JOB_RUNNING;
    printf("[%d] Running %s &\n", job->id, job->command);
}

void bring_job_foreground(int job_id)
{
    Job *job = get_job(job_id);

    if (job == NULL)
    {
        printf("No such job: %d\n", job_id);
        return;
    }

    pid_t pgid = job->pgid;
    int status;
    int was_stopped = (job->state == JOB_STOPPED);

    if (interactive && tcsetpgrp(STDIN_FILENO, pgid) < 0)
        perror("tcsetpgrp");

    if (was_stopped && kill(-pgid, SIGCONT) < 0)
    {
        perror("fg");
        restore_shell_terminal();
        return;
    }

    do
    {
        pid_t result = waitpid(-pgid, &status, WUNTRACED);
        if (result >= 0)
            break;

        if (errno != EINTR)
        {
            if (errno != ECHILD)
                perror("fg waitpid");
            restore_shell_terminal();
            remove_job(job_id);
            return;
        }
    }
    while (1);

    restore_shell_terminal();

    job = get_job(job_id);
    if (job == NULL)
        return;

    if (WIFSTOPPED(status))
    {
        job->state = JOB_STOPPED;
        printf("[%d]+ Stopped %s\n", job->id, job->command);
    }
    else
    {
        remove_job(job_id);
    }
}
