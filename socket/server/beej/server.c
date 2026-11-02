//This is program from beejs' networking guide page 36
//A TCP server that sends the message "Hello World" to a client who is connected via localhost port 3490

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3409"
#define BACKLOG 10

void sigchld_handler(int s){
    (void)s;    //Quiet unused variable warning
    //waitpid() might overwrite errno, so we save and restore it
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)-> sin6_addr);
}

int main(void){

    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;     //sockaddr_storage as we dont know the client connection is IPv4 or IPv6
    socklen_t sin_size;
    struct sigaction sa;    //Check this!!!
    int yes = 1;
    char s[INET6_ADDRSTRLEN];   //macro from <arpa/inet.h>
    int rv; 

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    //Loop through the linked list provided by getaddrinfo()
    for(p = servinfo; p != NULL; p = p->ai_next){
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("server: socket");           
            continue;   //If an error occurs in a node of the linked list, skip it and try the next node
        }
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            perror("setsockopt");
            exit(1);
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            perror("server: bind");
            continue;   //If error in binding skip the current node and move onto the next
        }
        break;  //If socket() and bind() succeed break out of the for loop
    }

    freeaddrinfo(servinfo);     //All done with this struct
    
    if(p == NULL){
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if(listen(sockfd, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;    //reap all dead processors
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD, &sa, NULL) == -1){
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1){
        sin_size = sizeof(their_addr);
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if(new_fd == -1){
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
        printf("server: got connection from: %s\n", s);

        if(!fork()){
            close(sockfd);  //Close primary socket for child
            if(send(new_fd, "Hello, world!\n", 15, 0) == -1){
                perror("send");
            }
            close(new_fd);  //Close secondary socket for child
            exit(0);
        }
        close(new_fd); //Close secondary socket for parent
    }
return 0;
}