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

typedef struct 
{
    uint32_t client_id;
    uint32_t barber_id;   
}data_t;

data_t* g_data;    // global data

typedef struct
{
    uint32_t client_id;
    sem_t* sem;
}costumer_t;

#define MAX 5
#define CLIENT_RATE 40               // client arrive rate
std::queue<costumer_t> q_wait;       // costumer FIFO
int wait_chairs;
//pthread_mutex_t barbeiros[3];      // Barbeiros - dormindo/acordado  
sem_t s_barbers[3];                  // sleeping=0/waked=1

sem_t s_wait;
sem_t sync_read;
sem_t sync_wakeup;                   // sync
sem_t sync_awake;
bool flag_wake[3];                   // flag

sem_t s_finish[3];
sem_t sync_queue_wait[MAX];
bool flag_queue[MAX];                // Maps what element in sync_queue_wait is available

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
    if(q_wait.size() >= MAX)
    {
        logger(LOG_WARNING, "FULL QUEUE! - Client %d leaves", client_id);
        return NULL;
    }
    costumer_t costumer = {.client_id = client_id, .sem=NULL};
    
    sem_wait(&s_wait);
    int i = 0;
    for(; i < 3; i++)
        if(!flag_wake[i]) break;
    
    // Prepare data to send to barber
    data_t data = {.client_id = client_id};

    if(i < 3) // if has at least one barber sleeping
    {
        g_data = &data;                     // send information to barber
        sem_post(&sync_wakeup);            // ask for some barber wake
        sem_wait(&sync_awake);             // wait some barber to wakeup (wait barber id)

        data.barber_id = g_data->barber_id;
        sem_post(&s_wait);  // release g_data access

        // logger(LOG_INFO, "[Client %d] Barber %d is cutting my hair", client_id, data.barber_id);
        // if(i != data.barber_id) logger(LOG_ERROR, "[Client %d] wasn't correct assigned %d !- %d");
    }
    else
    {
        sem_post(&s_wait);  // release g_data access
        // ensure thread safe for queue operation
        sem_wait(&s_queue);
        int j = 0;
        for(; j < MAX; j++)
            if(!flag_queue[j]) break;

        if(j >= MAX) 
        {
            logger(LOG_ERROR, "NO QUEUE SPACE j=%d", j);
        }
        else
        {
            costumer.sem = &sync_queue_wait[j];
            q_wait.push(costumer);
            logger(LOG_INFO, "[Client %d] is waiting (%d/%d)", client_id, q_wait.size(), MAX);
        }
        sem_post(&s_queue);

        // wait until barber calls
        sem_wait(costumer.sem);
        i = g_data->barber_id;
        // sem_post(&sync_read);
        // logger(LOG_DEBUG, "[Client %d] leaved queue, is going to cut", client_id);
    }
    

    sem_wait(&s_finish[i]);
    logger(LOG_INFO, "[Client %d] finish - barber %d", client_id, i);
    
    return NULL;
}

void *f_barber(void *arg)
{

    int id = *(int*) arg;
    data_t costumer;
    logger(LOG_INFO, "Barber %d created with pid %lu", id, (unsigned long)pthread_self());
    for(;;)
    {
        sem_wait(&sync_wakeup);
        flag_wake[id] = true;
        logger(LOG_INFO, "Barber %d waked", id);
        costumer = *g_data;  // get costumer from global buffer
        g_data->barber_id = id;
        sem_post(&sync_awake);

        for(;;)
        {
            logger(LOG_INFO, "Barber %d cutting client %d hair", id, costumer.client_id);
            // cut client hair
            sleep(10*T);
            sem_post(&s_finish[id]);
            logger(LOG_INFO, "Barber %d ENDED client %d hair", id, costumer.client_id);
            
            
            sem_wait(&s_queue);             // critical section to get client
            
                if(q_wait.size() == 0) break;   // no client waiting
                costumer.client_id = q_wait.front().client_id;
                q_wait.pop();
                // sem_wait(&s_wait);
                    g_data->barber_id = id;
                    sem_post(q_wait.front().sem);
                    // sem_wait(&sync_read);
                // sem_wait(&s_wait);
            
            sem_post(&s_queue);

        }
        logger(LOG_WARNING, "Barber %d going to sleep", id);
        flag_wake[id] = false;
    }
    return NULL;
}

int main()
{

    pthread_t thread1;
    pthread_t thread2;
    pthread_t t_baber[3];
    
    sem_init(&sync_read, 0, 0);
    sem_init(&s_queue, 0, 1);
    sem_init(&s_wait, 0, 1);
    if(sem_init(&sync_wakeup, 0, 0) != 0) logger(LOG_ERROR, "sync_wakeup");
    if(sem_init(&sync_awake, 0, 0) != 0) logger(LOG_ERROR, "sync_awake");

    uint32_t barber_ids[] = {0, 1, 2};
    for(int i = 0; i < 5; i++)
    {
        flag_queue[i] = false;
        sem_init(&sync_queue_wait[i], 0, 0);
    }
    for(int i = 0; i < 3; i++)
    {
        sem_init(&s_finish[i], 0, 0);
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
