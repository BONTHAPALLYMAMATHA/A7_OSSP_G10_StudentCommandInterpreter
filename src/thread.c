#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "../include/thread.h"

#define ITERATIONS 100000

static int counter = 0;

static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *worker(void *arg)
{
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++)
    {
        pthread_mutex_lock(&counter_mutex);

        counter++;

        pthread_mutex_unlock(&counter_mutex);
    }

    return NULL;
}

static void *monitor(void *arg)
{
    (void)arg;

    while (1)
    {
        sleep(10);

        pthread_mutex_lock(&counter_mutex);

        int current_count = counter;

        pthread_mutex_unlock(&counter_mutex);

        printf("\n[Monitor] ShellForge Running... Shared Counter: %d\n",
               current_count);

        fflush(stdout);
    }

    return NULL;
}

void run_thread_demo(void)
{
    pthread_t thread1;
    pthread_t thread2;

    counter = 0;

    if (pthread_create(&thread1, NULL, worker, NULL) != 0)
    {
        perror("pthread_create");
        return;
    }

    if (pthread_create(&thread2, NULL, worker, NULL) != 0)
    {
        perror("pthread_create");
        pthread_join(thread1, NULL);
        return;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("[Thread Demo] Shared counter: %d\n", counter);
}

void start_monitor_thread(void)
{
    pthread_t tid;

    if (pthread_create(&tid, NULL, monitor, NULL) != 0)
    {
        perror("pthread_create");
        return;
    }

    pthread_detach(tid);
}
