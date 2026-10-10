#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
static int unsafe_mode = 0;

static void *worker1(void *arg)
{
    (void)arg;

    pthread_mutex_lock(&lock1);
    printf("Worker 1: acquired Lock 1\n");
    fflush(stdout);
    usleep(200000);

    printf("Worker 1: waiting for Lock 2...\n");
    fflush(stdout);
    pthread_mutex_lock(&lock2);

    printf("Worker 1: acquired Lock 2\n");
    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);
    return NULL;
}

static void *worker2(void *arg)
{
    (void)arg;

    if (unsafe_mode)
    {
        pthread_mutex_lock(&lock2);
        printf("Worker 2: acquired Lock 2\n");
        fflush(stdout);
        usleep(200000);

        printf("Worker 2: waiting for Lock 1...\n");
        fflush(stdout);
        pthread_mutex_lock(&lock1);
    }
    else
    {
        /* Prevention: always acquire Lock 1 before Lock 2. */
        pthread_mutex_lock(&lock1);
        printf("Worker 2: acquired Lock 1\n");
        fflush(stdout);

        pthread_mutex_lock(&lock2);
    }

    printf("Worker 2: acquired both locks\n");
    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t1, t2;

    unsafe_mode = (argc > 1 && strcmp(argv[1], "unsafe") == 0);

    if (unsafe_mode)
        printf("Deadlock demonstration: unsafe lock ordering\n");
    else
        printf("Deadlock prevention: consistent lock ordering\n");

    fflush(stdout);

    if (pthread_create(&t1, NULL, worker1, NULL) != 0)
    {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    if (pthread_create(&t2, NULL, worker2, NULL) != 0)
    {
        perror("pthread_create");
        pthread_cancel(t1);
        pthread_join(t1, NULL);
        return EXIT_FAILURE;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);

    printf("Completed successfully.\n");
    return EXIT_SUCCESS;
}
