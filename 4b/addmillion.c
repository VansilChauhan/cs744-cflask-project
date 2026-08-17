#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <assert.h>

double GetTime()
{
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double)t.tv_sec + (double)t.tv_usec / 1e6;
}

int account_balance = 0;
pthread_mutex_t queue_mutex_lock;

void *increment(void *number_of_million)
{
    int number_of_million_int = *(int *)number_of_million;
    for (int j = 0; j < number_of_million_int; j++)
    {
        pthread_mutex_lock(&queue_mutex_lock);
        for (int i = 0; i < 1000000; i++)
        {
            account_balance++;
        }
        pthread_mutex_unlock(&queue_mutex_lock);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: ./<file> <thread_count>\n");
        exit(EXIT_FAILURE);
    }
    double start_time = GetTime();
    pthread_mutex_init(&queue_mutex_lock, NULL);
    int threadNum = atoi(argv[1]);
    int num_of_million = 2048 / threadNum;

    pthread_t th[threadNum];
    int i;
    for (i = 0; i < threadNum; i++)
    {
        if (pthread_create(th + i, NULL, &increment, &num_of_million) != 0)
        {
            perror("Failed to create thread");
            return 1;
        }
    }
    for (i = 0; i < threadNum; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            return 2;
        }
    }
    pthread_mutex_destroy(&queue_mutex_lock);
    double end_time = GetTime();
    printf("Time spent: %.2lf ms\n", (end_time - start_time) * 1000);
    return 0;
}