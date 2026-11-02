#include <stdio.h>
#include <poll.h>

/*
    struct pollfd{
        int fd;             //socket descriptor
        short events;       //bitmap of events we're interested in
        short revents;      //on return, bitmap of events that occured
    }
*/

int main(void){
    struct pollfd pfds[1];      //Initializing the pdfs[] array

    pfds[0].fd = 0;
    pfds[0].events = POLLIN;    
    
    //POLLIN  - Alert me when data is ready to recv() on this socket
    //POLLOUT - Alert me when I can send() data to this socket without blocking
    //POLLHUP - Alert me when the remote closed the connection

    printf("Hit RETURN or wait 2.5 seconds for timeout\n");

    int num_event = poll(pfds, 1, 2500);

    if(num_event = 0){
        printf("Poll timed out\n");
    }
    else{
        int poll_happened = pfds[0].revents & POLLIN;

        if(poll_happened){
            printf("File descriptor %d is ready to read\n", pfds[0].fd);
        }
        else{
            printf("Unexpected error occured: %d\n", pfds[0].revents);
        }
    }
    return 0;
}