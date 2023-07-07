#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CAPACITY 10 
#define MAX_PASSENGERS 20 

sem_t mutex; 
sem_t bus_arrived; 
sem_t passenger_boarded; 
int waiting_count = 0; 

void board()
{
    printf("Passenger boarded the bus.\n");
}

void depart()
{
    printf("Bus departed with %d passengers.\n", MAX_CAPACITY);
}

void showWaitingPassengers()
{
    printf("Waiting passengers: %d\n", waiting_count);
}

void* bus_thread(void* arg)
{
    while (1) {
        sem_wait(&mutex);

        if (waiting_count > 0) {

            int passengers_to_board = (waiting_count < MAX_CAPACITY) ? waiting_count : MAX_CAPACITY;
            for (int i = 0; i < passengers_to_board; i++) {
                sem_post(&bus_arrived);
                sem_wait(&passenger_boarded);
            }

            waiting_count -= passengers_to_board;
            sem_post(&mutex);
            depart();
            showWaitingPassengers();
        } else {
            sem_post(&mutex);
            sleep(1);
        }
    }
}

void* passenger_thread(void* arg)
{
    sem_wait(&mutex);
    waiting_count++;
    sem_post(&mutex);

    sem_wait(&bus_arrived);
    board();
    sem_post(&passenger_boarded);

    pthread_exit(NULL);
}

int main()
{
    sem_init(&mutex, 0, 1);
    sem_init(&bus_arrived, 0, 0);
    sem_init(&passenger_boarded, 0, 0);

    pthread_t bus_thread_id;
    pthread_t passenger_threads[MAX_PASSENGERS];

    if (pthread_create(&bus_thread_id, NULL, bus_thread, NULL) != 0) {
        printf("Failed to create bus thread.\n");
        return 1;
    }

    for (int i = 0; i < MAX_PASSENGERS; i++) {
        if (pthread_create(&passenger_threads[i], NULL, passenger_thread, NULL) != 0) {
            printf("Failed to create passenger thread.\n");
            return 1;
        }

    }

    for (int i = 0; i < MAX_PASSENGERS; i++) {
        if (pthread_join(passenger_threads[i], NULL) != 0) {
            printf("Failed to join passenger thread.\n");
            return 1;
        }
    }

    if (pthread_join(bus_thread_id, NULL) != 0) {
        printf("Failed to Open theread\n");
        return 1;
    }

    sem_destroy(&mutex);
    sem_destroy(&bus_arrived);
    sem_destroy(&passenger_boarded);

    return 0;
}