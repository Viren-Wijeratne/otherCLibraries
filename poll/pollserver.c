#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#define PORT "9034"

//Function to convert the client IP into a dotted format, regardless if the IP address is IPv4 or IPv6
const char *inet_ntops2(void *addr, char *buf, size_t size){
    struct sockaddr_storage *sas = addr;
    struct sockaddr_in *sa4;
    struct sockaddr_in6 *sa6;
    void *src;

    switch(sas->ss_family){
        case AF_INET:
            sa4 = addr;
            src = &(sa4->sin_addr);
            break;

        case AF_INET6:
            sa6 = addr;
            src = &(sa6->sin6_addr);
            break;

        default:
            return NULL;
    }

    return inet_ntop(sas->ss_family, src, buf, size);
}

//Define new listener socket and bind it to a port
int get_listener_socket(void){
    int listener;
    int yes = 1;
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0){
        fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next){
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(listener < 0){
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if(bind(listener, p->ai_addr, p->ai_addrlen) < 0){
            close(listener);
            continue;
        }
        break;
    }

    if(p == NULL){
        return -1;
    }

    freeaddrinfo(ai);

    if(listen(listener, 10) == -1){
        return -1;
    }
    return listener;
}

//If the size of the pfds[] array is not enough, rellocate the heap size to accomadate 2 more pfds in the array 
//Initially code runs when fd_count goes to 5, but at first it can only go upto 4 as the fd_size is 5 and the array only has maximum upto the 4th index(as the array is zero-indexed), so if the fd_count increases beyond the available index this code runs, then fd_count == fd_size
void add_to_pfds(struct pollfd **pfds, int newfd, int *fd_count, int *fd_size){
    if(*fd_count == *fd_size){  //This happens when the pfds[] becomes full
        *fd_size *= 2;          //IF full increase size by 2
        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;      //Initializing the new pfds
    (*pfds)[*fd_count].events = POLLIN; //Look at *fd-count it has not been increased in any way, but because the array is zero index
    (*pfds)[*fd_count].revents = 0;

    (*fd_count)++;
}

//Used later to remove any closed connections from the polling process
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count){
    pfds[i] = pfds[*fd_count - 1];  //Decrement the fd_count 
    (*fd_count)--;
}

//Handle new connection if size of pfds[] is not enough defined by fd-size, rellocate memory for extra 2 more pfds entries
void handle_new_connection(int listener, int *fd_count, int *fd_size, struct pollfd **pfds){
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    int newfd;
    char remoteIP[INET6_ADDRSTRLEN];

    addrlen = sizeof(remoteaddr);
    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

    if(newfd == -1){
        perror("accept");
    }
    else{
        add_to_pfds(pfds, newfd, fd_count, fd_size);    //fd_count = fd_size for allocate more space for 2 in heap

        printf("pollserver: new connection from %s on socket %d\n", inet_ntops2(&remoteaddr, remoteIP, sizeof(remoteIP)), newfd);
    }
}

void handle_client_data(int listener, int *fd_count, struct pollfd *pfds, int *pfd_i){
    char buf[256];
    int nbytes = recv(pfds[*pfd_i].fd, buf, sizeof(buf), 0);
    int sender_fd = pfds[*pfd_i].fd;

    if(nbytes <= 0){
        if(nbytes == 0){
            printf("pollserver: socket %d hung up\n", sender_fd);   //The client called quit in telnet
        }
        else{
            perror("recv");
        }
        //If the client quits close the fd and decrement pfd_i
        close(pfds[*pfd_i].fd);

        del_from_pfds(pfds, *pfd_i, fd_count);  //As the function states delete a pfd that was closed above

        (*pfd_i)--; //derefer and decrement
    }
    else{
        printf("pollserver: recv from fd %d: %.*s", sender_fd, nbytes, buf);

        for(int j = 0; j < *fd_count; j++){
            int dest_fd = pfds[j].fd;   //Iterate through all the fd's each cycle
            
            if(dest_fd != listener && dest_fd != sender_fd){    //If the fd does not belong to the, fd that is sending the data or the listener, send the data in the buffer
                if(send(dest_fd, buf, nbytes, 0) == -1){        //As the listener is the primary socket, and in a default setup listener fd would be 3, which will receive no communication
                    perror("send");
                }
            }
        }
    }

}

void process_connections(int listener, int *fd_count, int *fd_size, struct pollfd **pfds){
    for(int i = 0; i < *fd_count; i++){
        if((*pfds)[i].revents & (POLLIN | POLLHUP)){
            if((*pfds)[i].fd == listener){
                handle_new_connection(listener, fd_count, fd_size, pfds);   //Create new connection if we are the listener
            }
            else{
                handle_client_data(listener, fd_count, *pfds, &i);
            }
        }
    }
}

int main(void){

    int listener;

    int fd_size = 5;
    int fd_count = 0;
    struct pollfd *pfds = malloc(sizeof(*pfds) * fd_size);

    listener = get_listener_socket();

    if(listener == -1){
        fprintf(stderr, "error getting listeneing socket\n");
        exit(1);
    }

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1;

    puts("pollserver: waiting for connections...");

    for(;;){
        int poll_count = poll(pfds, fd_count, -1);

        if(poll_count == -1){
            perror("poll");
            exit(1);
        }
        process_connections(listener, &fd_count, &fd_size, &pfds);
    }
    free(pfds);
    return 0;
}
/*
main
|   set fd_size and fd_count, also allocate memory in heap for the initial fd_size
|
V
call get_listener_socket()
|   first pfds set to index 0, set listener as .fd and check for POLLIN as .event
|   increase fd_count to 1
V
in a loop call:         existing?                          send data?
process_connections() ---------> call handle_client_data()---------> send()
|                                       |  Client quit? 
|   new?                                ---------------> del_from_pfds()
V
call handle_new_connection()---------------------> add_to_pfds()
|                           if fd_count = fd_size
| if fd_count != fd_size
V
accept()














*/