#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t global_var_lock;
pthread_mutex_t queue_lock;
pthread_cond_t task_available;

// Global variables
long sum = 0;
long odd = 0;
long even = 0;
long min = INT_MAX;
long max = INT_MIN;
bool done = false;

typedef struct
{
    char type;
    int num;
} Task;

// queue logic

typedef struct TaskNode
{
    Task task;
    struct TaskNode *next;
} TaskNode;

typedef struct
{
    TaskNode *front;
    TaskNode *rear;
    int size;
} TaskQueue;

TaskQueue queue;

//-----------------------------------------

TaskNode *create_task_node(Task task)
{
    TaskNode *new_node = (TaskNode *)malloc(sizeof(TaskNode));
    new_node->task = task;
    new_node->next = NULL;
    return new_node;
}

void enqueue_task(Task task)
{
    TaskNode *new_node = create_task_node(task);
    pthread_mutex_lock(&queue_lock);
    if (queue.rear == NULL)
    {
        queue.front = queue.rear = new_node;
    }
    else
    {
        queue.rear->next = new_node;
        queue.rear = new_node;
    }
    queue.size++;
    pthread_cond_signal(&task_available);
    pthread_mutex_unlock(&queue_lock);
}

Task dequeue_task()
{
    pthread_mutex_lock(&queue_lock);
    while (queue.size == 0)
    {
        pthread_cond_wait(&task_available, &queue_lock);
    }
    TaskNode *temp = queue.front;
    Task task = temp->task;
    queue.front = queue.front->next;
    if (queue.front == NULL)
    {
        queue.rear = NULL;
    }
    free(temp);
    queue.size--;
    pthread_mutex_unlock(&queue_lock);
    return task;
}

void processtask(long number)
{
    // simulate burst time
    sleep(number);

    pthread_mutex_lock(&global_var_lock);
    // update global variables
    sum += number;
    if (number % 2 == 1)
    {
        odd++;
    }
    else
    {
        even++;
    }
    if (number < min)
    {
        min = number;
    }
    if (number > max)
    {
        max = number;
    }
    pthread_mutex_unlock(&global_var_lock);
}

void *thread_work()
{
    while (1)
    {
        Task task = dequeue_task();

        if (task.type == 'e')
        {
            break;
        }

        if (task.type == 'p')
        { // processing task
            processtask(task.num);
            printf("Task completed\n");
        }
        else if (task.type == 'w')
        { // waiting period
            sleep(task.num);
            printf("Wait Over\n");
        }
        else
        {
            printf("ERROR: Type Unrecognizable: '%c'\n", task.type);
            exit(1);
        }
    }
}

void reset_queue()
{
    pthread_mutex_init(&queue_lock, NULL);
    queue.front = NULL;
    queue.rear = NULL;
    queue.size = 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: sum <infile> <thread_count>\n");
        exit(EXIT_FAILURE);
    }
    char *fn = argv[1];
    int thread_count = atoi(argv[2]);

    pthread_mutex_init(&global_var_lock, NULL);

    pthread_cond_init(&task_available, NULL);
    // Read from file
    FILE *fin = fopen(fn, "r");
    long t;
    fscanf(fin, "%ld\n", &t);
    printf("The number of tasks are : %ld \n", t);

    reset_queue();

    pthread_t threads[thread_count];
    for (int i = 0; i < thread_count; i++)
    {
        pthread_create(&threads[i], NULL, &thread_work, NULL);
    }

    char type;
    long num;
    while (fscanf(fin, "%c %ld\n", &type, &num) == 2)
    {
        Task task = {type, num};
        enqueue_task(task);
    }
    for (int i = 0; i < thread_count; i++)
    {
        Task exit_task = {'e', 0};
        enqueue_task(exit_task);
    }
    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }
    fclose(fin);
    // Print global variables
    printf("%ld %ld %ld %ld %ld\n", sum, odd, even, min, max);
    pthread_mutex_destroy(&global_var_lock);
    pthread_mutex_destroy(&queue_lock);
    pthread_cond_destroy(&task_available);
    return 0;
}
