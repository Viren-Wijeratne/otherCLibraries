/*Program from beejs' network guide page 23
Show the IP address of a host regardless of the IP is IPv4 or IPv6
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[]){

    struct addrinfo hints, *res, *p;
    int status;
    char ipstring[INET6_ADDRSTRLEN];

    if(argc != 2){
        fprintf(stderr, "Usage: showip <hostname>\n");
        return 1;
    }

    memset(&hints, 0, sizeof(hints));   //Clears the memory of hints
    hints.ai_family = AF_UNSPEC;        //As this program works as a client we dont know the ai_family that will be returned back so we use AF_UNSPEC
    hints.ai_socktype = SOCK_STREAM;

    //Returns a pointer to linked list res, the specifics of request is done using the first 3 arguements of the function getaddrinfo() 
    if((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP addresses for %s:\n\n", argv[1]);

    //Each time the loop runs it checks if the node of the link list is AF_INET or AF_INET6 then runs the specifc if statement to save the data within the ai_addr to a more accessible sin_addr
    for(p = res; p != NULL; p = p->ai_next){    //Iterate through each of the nodes returned by getaddrinfo for that specific hostname specificied by argv[1]
        void *addr;
        char *ipversion;
        struct sockaddr_in *ipv4;
        struct sockaddr_in6 *ipv6;

        if(p->ai_family == AF_INET){        //Group the nodes according to there ai_family to IPv4 and IPv6
            ipv4 = (struct sockaddr_in *)p->ai_addr;    //Cast the result to sockaddr_in so it can be accessed easily via sin_addr
            addr = &(ipv4->sin_addr);
            ipversion = "IPv4";
        }else{
            ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipversion = "IPv6";
        }

        inet_ntop(p->ai_family, addr, ipstring, sizeof(ipstring));  //Convert the address to a dot notation for presentation with printf
        printf("  %s: %s\n", ipversion, ipstring);
    }
    freeaddrinfo(res);

    return 0;
}