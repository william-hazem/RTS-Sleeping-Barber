#include <stdio.h>
#include <pthread.h>
#include <unistd.h>     // sleep
#include <logger.h>     // logger utilities
#include <semaphore.h>  // sem_t, sem_post, sem_wait
#include <math.h>       // rand
#include <queue>        // std::queue
typedef struct 
{
    int client_id;
    int barber_id;   
}data_t;

data_t* g_data;    // global data

typedef struct
{
    int client_id;
    sem_t* sem;
}costumer_t;

#define MAX 5
#define CLIENT_RATE 20               // client arrive rate
std::queue<costumer_t> q_wait;       // costumer FIFO

sem_t s_wait;
sem_t sync_read;
sem_t sync_wakeup;                   // sync
sem_t sync_awake;
bool flag_wake[3];                   // flag

sem_t s_finish[3];                   // sync: says to client that the barber finished
sem_t s_chair[3];                    // sync: wait client leave
sem_t sync_queue_wait[MAX];
bool flag_queue[MAX];                // Maps what element in sync_queue_wait is available

sem_t s_queue;

#define T 1                          // Time constant

void *f_client(void *arg);

void *f_barbershop(void *arg)
{   
    // randonly generate clients
    int client_counter = 0;
    for(;;)
    {
        int r = rand() % 100;
        if(r >= 100 - CLIENT_RATE)
        {
            pthread_t tid;
            logger(LOG_DEBUG, "Client %d arrived", client_counter);
            int temp = client_counter;
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
    int client_id = *(int*)arg;
    if(q_wait.size() >= MAX)
    {
        logger(LOG_WARNING, "FULL QUEUE! - Client %d leaves", client_id);
        return NULL;
    }
    costumer_t costumer = {.client_id = client_id, .sem=NULL,};
    
    sem_wait(&s_wait);
    int i = 0;
    for(; i < 3; i++)
        if(!flag_wake[i]) break;
    
    // Prepare data to send to barber
    data_t data = {.client_id = client_id};

    if(i < 3) // if has at least one barber sleeping
    {
        logger(LOG_DEBUG, "[Client %d] Trying to wakeup a barber", client_id);

        g_data = &data;                     // send information to barber
        sem_post(&sync_wakeup);            // ask for some barber wake
        sem_wait(&sync_awake);             // wait some barber to wakeup (wait barber id)

        data.barber_id = g_data->barber_id;
        sem_post(&s_wait);  // release g_data access

        logger(LOG_DEBUG, "[Client %d] Barber %d is cutting my hair", client_id, data.barber_id);
        // if(i != data.barber_id) logger(LOG_ERROR, "[Client %d] wasn't correct assigned %d !- %d");
    }
    else // all barber busy - wait on queue
    {
        sem_post(&s_wait);  // release g_data access
        // ensure thread safe for queue operation
        sem_wait(&s_queue);
        logger(LOG_DEBUG, "[Client %d] Trying to enter into queue", client_id);
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
            logger(LOG_DEBUG, "[Client %d] is waiting (%d/%d)", client_id, q_wait.size(), MAX);
        }
        sem_post(&s_queue);

        // wait until barber calls
        sem_wait(costumer.sem);
        data.barber_id = g_data->barber_id;
        // sem_post(&sync_read);
        logger(LOG_DEBUG, "[Client %d] leaved queue, sitting in chair %d", client_id, data.barber_id);
    }
    
    /// Request the chair when available
    //  only grant access to barber's chair  when the last costumer leave it
    sem_wait(&s_chair[data.barber_id]);
    
    sem_wait(&s_finish[i]);
    logger(LOG_DEBUG, "[Client %d] finish - barber %d", client_id, data.barber_id);

    sem_post(&s_chair[data.barber_id]);
    return NULL;
}

void *f_barber(void *arg)
{
    int id = *(int*) arg;   // Barber id
    data_t costumer;        // Store costumer id and semaphore
    logger(LOG_INFO, "[Barber %d] created with pid %lu", id, (unsigned long)pthread_self());
    for(;;)
    {
        sem_wait(&sync_wakeup);
        flag_wake[id] = true;
        logger(LOG_INFO, "[Barber %d] waked", id);
        costumer = *g_data;  // get costumer from global pointer
        g_data->barber_id = id;
        sem_post(&sync_awake);

        for(;;)
        {
            logger(LOG_INFO, "[Barber %d] cutting client %d hair", id, costumer.client_id);
            sleep(5*T);                    // simulate the hair cut
            sem_post(&s_finish[id]);
            logger(LOG_INFO, "[Barber %d] ENDED client %d hair", id, costumer.client_id);

            if(q_wait.size() == 0) break;   // no client waiting
            // ensure thread safe for queue operation
            sem_wait(&s_queue);             // critical section to get costumer
            costumer.client_id = q_wait.front().client_id;
            g_data->barber_id = id;         // send barber id to client
            sem_post(q_wait.front().sem);
            q_wait.pop();                   // remove costumer from queue
            
            sem_post(&s_queue);

        }
        logger(LOG_WARNING, "[Barber %d] going to sleep", id);
        flag_wake[id] = false;
    }
    return NULL;
}

int main()
{
    logger_init();
    
    pthread_t t_barbershop;
    pthread_t t_baber[3];
    int barber_ids[] = {0, 1, 2};
    
    // initialize semaphores
    sem_init(&sync_read, 0, 0);
    sem_init(&s_queue, 0, 1);
    sem_init(&s_wait, 0, 1);
    sem_init(&sync_wakeup, 0, 0);
    sem_init(&sync_awake, 0, 0);

    // initialize queue flags
    for(int i = 0; i < 5; i++)
    {
        flag_queue[i] = false;
        sem_init(&sync_queue_wait[i], 0, 0);
    }
    // initialize barber tasks
    for(int i = 0; i < 3; i++)
    {
        sem_init(&s_finish[i], 0, 0);
        sem_init(&s_chair[i], 0, 1);
        if(pthread_create(&t_baber[i], NULL, f_barber, &barber_ids[i]) != 0)
        {
            logger(LOG_ERROR, "Error when creating barber tasks");
            exit(1);
        }
    }
    
    sleep(1);
    // initialize barbershop (costumer thread spawner)
    pthread_create(&t_barbershop, NULL, f_barbershop, NULL);

    pthread_exit(NULL);
    return 0; 
}
