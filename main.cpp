#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // sleep
#include <time.h>
#include <logger.h>
#include <semaphore.h> 
#include <math.h>
#include <inttypes.h>
#include <queue>

#define MAX 5
std::queue<int> q_wait;              // FIFO de espera
//pthread_mutex_t barbeiros[3];      // Barbeiros - dormindo/acordado  
sem_t s_barbeiros[3];

#define T 1                          // Time constant

void *f_barbershop(void *arg)
{   
    // randonly generate clients
    uint32_t client_counter = 0;
    for(;;)
    {
        int r = rand() % 100;
        if(r >= 80)
        {
            // logger(LOG_DEBUG, "Client arrived");
            if(q_wait.size() < MAX)
            {
                logger(LOG_INFO, "Client waiting (%d)", client_counter);
                q_wait.push(client_counter);
                client_counter++;
            } else
                logger(LOG_WARNING, "No chairs to wait");
            
        }

        sleep(T);
    }
    return NULL;
}

void *f_barber(void *arg)
{
    static int id = 0;
    logger(LOG_INFO, "Barber %d created with pid %lu", id++, (unsigned long)pthread_self());
    for(;;)
    {

        sleep(T);
    }
    printf("Thread: Hello World! \n");
    return NULL;
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

    pthread_t thread1;
    pthread_t thread2;
    pthread_t t_baber[3];

    pthread_create(&thread1, NULL, f_barbershop, NULL);
    for(int i = 0; i < 3; i++)
        if(pthread_create(&t_baber[i], NULL, f_barber, NULL) != 0)
            logger(LOG_ERROR, "Error when creating barber tasks");

    pthread_create(&thread2, NULL, thread_function2, NULL);

    pthread_exit(NULL);
    return 0; 
}
