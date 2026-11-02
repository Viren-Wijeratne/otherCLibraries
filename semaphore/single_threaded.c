#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Declare a global semaphore
sem_t my_semaphore;

void* worker_thread(void* arg) {
    printf("Worker: Doing some heavy work...\n");
    sleep(2); // Simulate work
    printf("Worker: Work finished!\n");

    // Signal (increment) the semaphore to wake up the main thread
    sem_post(&my_semaphore);    //Set semaphore to be incremented 
    
    return NULL;
}

int main() {
        pthread_t thread_id;

        // Initialize the semaphore
        // 0: Shared between threads of the same process
        // 0: Initial value of the semaphore (starts locked)
        if (sem_init(&my_semaphore, 0, 0) != 0) {
            perror("Semaphore initialization failed");
            return 1;
        }

        // Create the worker thread
        pthread_create(&thread_id, NULL, worker_thread, NULL);

    /*Here the initial setup is already done, semaphore set to 0 and the worker thread has started*/

        printf("Main: Waiting for worker thread to finish...\n");

    // Wait (decrement) on the semaphore by main thread. Blocks because initial value is 0.
    sem_wait(&my_semaphore);    //main threads function freeze after calling this

    printf("Main: Worker is done. Proceeding with main execution.\n");
    sleep(2);   //Simulate work done by the main thread

    // Clean up
    pthread_join(thread_id, NULL);
    sem_destroy(&my_semaphore);

    return 0;
}

/*

- When the worker thread is callled it does not change any values of the semaphore, but we have already set the semaphore value to be 0 at initialization which happens before the worker thread was created, So naturally the worker thread starts executing with the semaphore set to 0, which in turn blocks the main thread from running when the work thread is still active

- On line 39 we see that the main thread tries to call sem_wait() but because the semaphore is already 0 and the worker thread is already running the request from the main thread will be blocked until the worker thread calls sem_post()

- When the worker threat has finished its function it calls the sem_post() which sets the semaphore to be 1, now the main thread who was in the queue could use this oppotunity to use the CPU

- By initializing it to 0, we are deliberately setting up a "trap" for the main thread, forcing it to stop until the worker thread explicitly releases it using sem_post(). This is why this specific pattern is called a signaling mechanism or a rendezvous, rather than a mutual exclusion lock

- Search for producer-consumer model
*/