#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // sleep
#include <time.h>
#include <logger.h>

void *thread_function(void *arg)
{
    logger(LOG_INFO, "Thread 1 created with pid %lu", (unsigned long)pthread_self());
    printf("Thread: Hello World! \n");
}

void *thread_function2(void *arg)
{
    clock_t c = clock();
    sleep(1);
    float seconds = (float)(clock() - c) / CLOCKS_PER_SEC;
    printf("Thread: %lf\n", seconds);
    return NULL;
}

int main()
{
    logger(LOG_DEBUG,   "Debug text");
    logger(LOG_INFO,    "Info text");
    logger(LOG_WARNING, "Warning text");
    logger(LOG_ERROR,   "Error text");
    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, NULL, thread_function, NULL);
    pthread_create(&thread2, NULL, thread_function2, NULL);
    logger(LOG_INFO, "Thread 1 created with thread id %lu", (unsigned long)thread1);
    logger(LOG_INFO, "Thread 2 created with thread id %lu", (unsigned long)thread2);

    // Problem: How do we tell main() to wait for our thread
    // to execute ?
    pthread_exit(NULL);
    return 0; 
}
