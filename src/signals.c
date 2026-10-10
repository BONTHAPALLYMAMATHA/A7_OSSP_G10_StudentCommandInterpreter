#include "signals.h"

/*
 * Job control manages shell and child signal dispositions.
 * Do not reap children inside a SIGCHLD handler because
 * the job-control module needs their wait statuses.
 */
void initialize_signals(void)
{
    /* Signal configuration is performed by init_job_control(). */
}
