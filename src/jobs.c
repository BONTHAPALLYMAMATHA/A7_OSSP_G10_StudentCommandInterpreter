#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "jobs.h"

static Job jobs[MAX_JOBS];

void init_jobs(void)
{
    memset(jobs, 0, sizeof(jobs));
}

int add_job(pid_t pgid, const char *command, int state)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (jobs[i].id == 0)
        {
            jobs[i].id = i + 1;
            jobs[i].pgid = pgid;
            jobs[i].state = state;
            snprintf(jobs[i].command, sizeof(jobs[i].command),
                     "%s", command ? command : "");
            return jobs[i].id;
        }
    }

    fprintf(stderr, "Job table is full.\n");
    return -1;
}

void remove_job(int id)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (jobs[i].id == id)
        {
            memset(&jobs[i], 0, sizeof(jobs[i]));
            return;
        }
    }
}

Job *get_job(int id)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (jobs[i].id == id)
            return &jobs[i];
    }
    return NULL;
}

void update_job_status(void)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status,
                          WNOHANG | WUNTRACED | WCONTINUED)) > 0)
    {
        for (int i = 0; i < MAX_JOBS; i++)
        {
            if (jobs[i].id != 0 && jobs[i].pgid == pid)
            {
                if (WIFSTOPPED(status))
                    jobs[i].state = JOB_STOPPED;
                else if (WIFCONTINUED(status))
                    jobs[i].state = JOB_RUNNING;
                else if (WIFEXITED(status) || WIFSIGNALED(status))
                {
                    printf("\n[%d] Done %s\n",
                           jobs[i].id, jobs[i].command);
                    remove_job(jobs[i].id);
                }
                break;
            }
        }
    }
}

void list_jobs(void)
{
    update_job_status();

    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (jobs[i].id != 0)
        {
            const char *state = jobs[i].state == JOB_RUNNING
                                    ? "Running"
                                    : jobs[i].state == JOB_STOPPED
                                          ? "Stopped" : "Done";

            printf("[%d] %-8s %s\n",
                   jobs[i].id, state, jobs[i].command);
        }
    }
}
