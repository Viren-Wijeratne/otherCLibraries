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
void* drinks_alot(void* a){
    for(int i = 0; i < 100000; i++){
        beers = beers - 1;      //20 threads access and modify the same varibale so threads crash into each other
    }
    return NULL;
}

int main(){

    pthread_t threads[20];

    printf("%i bottles of beer on the wall\n%i bottles of beer\n", beers, beers);

    for(int t = 0; t < 20; t++){
        pthread_create(&threads[t], NULL, drinks_alot, NULL);
    }

    void* result;
    for(int t = 0; t < 20; t++){
        pthread_join(threads[t], &result);
    }

    printf("There are now %i bottles of beer on the wall\n", beers);

    return 0;
}