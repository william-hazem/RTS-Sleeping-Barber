#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // sleep
#include <time.h>
#include <logger.h>
#include <semaphore.h> 
#include <math.h>
#include <inttypes.h>
#include <queue>

// Problemas divertidos de experiência:
//  waiting for a resource infinitly because someone hold it and finished without release the resource
//

#define MAX 5
#define CLIENT_RATE 40               // client arrive rate
std::queue<int> q_wait;              // waiting FIFO
int wait_chairs;
//pthread_mutex_t barbeiros[3];      // Barbeiros - dormindo/acordado  
sem_t s_barbers[3];                  // sleeping=0/waked=1
sem_t s_queue;

#define T 1                          // Time constant

void *f_client(void *arg);

void *f_barbershop(void *arg)
{   
    // randonly generate clients
    uint32_t client_counter = 0;
    for(;;)
    {
        int r = rand() % 100;
        if(r >= 100 - CLIENT_RATE)
        {
            pthread_t tid;
            logger(LOG_DEBUG, "Client %d arrived", client_counter);
            uint32_t temp = client_counter;
            if(pthread_create(&tid, NULL, f_client, (void*)&temp) != 0)
                logger(LOG_ERROR, "Client thread not started");
            client_counter++;   
        }
        sleep(T);
    }
    return NULL;
}

void *f_client(void *arg)
{
    uint32_t client_id = *(uint32_t*)arg;
    for(;;)
    {
        
        if(q_wait.size() >= MAX)
        {
            logger(LOG_WARNING, "FULL QUEUE! - Client %d leaves", client_id);
            return NULL;
        }

        sem_wait(&s_queue);
        q_wait.push(client_id);
        logger(LOG_INFO, "CLient %d is waiting (%d/%d)", client_id, q_wait.size(), MAX);
        sem_post(&s_queue);

        /// PRECISA MELHORAR
        for(int i = 0; i < 3; i++)
        {
            int value;
            sem_getvalue(&s_barbers[i], &value); 
            // if(i == 2) logger(LOG_ERROR, "Barber v=%d", value);
            if(value == 0) { // barbers i is sleeping
                sem_post(&s_barbers[i]);    
                break;
            }
        }
        break;
        
        sleep(T);
    }
    return NULL;
}

void *f_barber(void *arg)
{

    int id = *(int*) arg;
    logger(LOG_INFO, "Barber %d created with pid %lu", id, (unsigned long)pthread_self());
    for(;;)
    {
        sem_wait(&s_barbers[id]);
        logger(LOG_INFO, "Barber %d waked", id);
        for(;;)
        {
            
            sem_wait(&s_queue);             // critical section to get client
            if(q_wait.size() == 0) break;   // no client waiting
            int client_id = q_wait.front();
            q_wait.pop();
            sem_post(&s_queue);

            logger(LOG_DEBUG, "Barber %d cutting client %d hair", id, client_id);
            // cut client hair
            sleep(10*T);
            logger(LOG_DEBUG, "Barber %d ENDED client %d hair", id, client_id);

        }
        logger(LOG_INFO, "Barber %d going to sleep", id);
        
    }
    return NULL;
}

int main()
{

    pthread_t thread1;
    pthread_t thread2;
    pthread_t t_baber[3];
    
    sem_init(&s_queue, 0, 1);
    uint32_t barber_ids[] = {0, 1, 2};
    for(int i = 0; i < 3; i++)
    {
        sem_init(&s_barbers[i], 0, 0);
        if(pthread_create(&t_baber[i], NULL, f_barber, &barber_ids[i]) != 0)
        {
            logger(LOG_ERROR, "Error when creating barber tasks");
            exit(1);
        }
    }
    sleep(1);
    pthread_create(&thread1, NULL, f_barbershop, NULL);

    pthread_exit(NULL);
    return 0; 
}
