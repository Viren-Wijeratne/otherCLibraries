#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

void error(char *msg){
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
}

void* does_not(void *a){        //Only functions that have a return type of void* can be used for creating a thread 
    for(int i = 0; i < 5; i++){
        sleep(1);
        puts("Does not!\n");
    }
    return NULL;
}

void* does_too(void *a){
    for(int i = 0; i < 5; i++){
        sleep(1);
        puts("Does too!\n");
    }
    return NULL;
}

int main(){

    pthread_t t0;
    pthread_t t1;

    //pthread_create() function starts a new thread in the calling process
    if(pthread_create(&t0, NULL, does_not, NULL) == -1){
        error("Can't create thread t0");
    }
    if(pthread_create(&t1, NULL, does_too, NULL) == -1){
        error("Can't create thread t1");
    }
/*The program needs to wait till the two threads have finished there work before exiting the process*/
    void* result;
    //pthread_join()  function  waits for the thread specified by thread to terminate.  If that thread has already terminated, then pthread_join() returns immediately
    if(pthread_join(t0, &result) == -1){
        error("Can't join thread t0");
    }
    if(pthread_join(t1, &result)){
        error("Can't join thread t1");
    }



    return 0;
}
//During compilation pthread library should be linked "gcc main.c -lpthread -o main"