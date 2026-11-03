#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

void error(char *msg){
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

int beers = 2000000;

pthread_mutex_t beers_lock = PTHREAD_MUTEX_INITIALIZER;

void* drinks_alot(void* a){
    int i;
    for(i = 0; i < 100000; i++){
        pthread_mutex_lock(&beers_lock);
        beers = beers - 1;      //20 threads access and modify the same varibale so threads crash into each other
        pthread_mutex_unlock(&beers_lock);
    }
    
    printf("beers = %i\n", beers);
    return NULL;
}

int main(){

    pthread_t threads[20];

    printf("%i bottles of beer on the wall\n%i bottles of beer\n", beers, beers);
    int t;
    for(t = 0; t < 20; t++){
        pthread_create(&threads[t], NULL, drinks_alot, NULL);
    }

    void* result;
    for(t = 0; t < 20; t++){
        pthread_join(threads[t], &result);
    }

    printf("There are now %i bottles of beer on the wall\n", beers);

    return 0;
}

/*
- The beers count without a mutex will not go to 0 as the treads will be crashing into each other so a sort of traffic light needs to introduced
- The traffic signals that prevent threads form crahsing into each other is called mutexes, it one of the ways to make code thread safe
- To protect a section of code, mutex lock needs to be created
    pthread_mutex_t a_lock = PTHREAD_MUTEX_INITIALIZER;
- The mutex needs to be visible to all of the threads that might crash into each other, so that we might need to define the mutex as a global variable
- PTHREAD_MUTEX_INITIALIZER is actually a macro
- pthread_mutex_lock(&a_lock); will let only one thread get past, all the other threads will have to wait when they get to it
- When the thread gets to the end of the sensitive code, it makes a call to pthread_mutex_unlock(&a_lock); That set the traffic signal back to green, and another thread is allowed onto the sensitive code
*/