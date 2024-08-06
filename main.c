#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // sleep
#include <time.h>
#include <logger.h>

void *thread_function(void *arg)
{
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
    printf("\033[33m OLA \033[37m\n");
    logger(LOG_DEBUG,   "Debug text\n");
    logger(LOG_INFO,    "Info text\n");
    logger(LOG_WARNING, "Warning text\n");
    logger(LOG_ERROR,   "Error text\n");
    pthread_t thread_id;

    pthread_create(&thread_id, NULL, thread_function, NULL);
    pthread_create(NULL, NULL, thread_function2, NULL);
    // Problem: How do we tell main() to wait for our thread
    // to execute ?
    pthread_exit(NULL);
    return 0; 
}