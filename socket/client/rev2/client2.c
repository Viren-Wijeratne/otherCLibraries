/*This client can only receive http pages
Execution = ./client2 <domain> <port>
Example = ./client2 example.com 80
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

void error(char *msg){
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

int open_socket(char *host, char *port){
    struct addrinfo *res;       //Used to store a pointer to a link list that contains DNS data
    struct addrinfo hints;
    int status;

    memset(&hints, 0, sizeof(hints));   //Making sure the hints struct is empty
    
    hints.ai_family = PF_UNSPEC;        //AF_UNSPEC=> Can be assigned with a IPv4 or IPv6 address
    hints.ai_socktype = SOCK_STREAM;

    /*
        int getaddrinfo(const char *restrict node,      //Can be a domain name or an ip address 
                       const char *restrict service,    //Can be port number(as string) or particular service from /etc/services 
                       const struct addrinfo *restrict hints,
                       struct addrinfo **restrict res);

        struct addrinfo {
            int ai_flags;               // AI_PASSIVE, AI_CANONNAME, etc.
            int ai_family;              // AF_INET, AF_INET6, AF_UNSPEC
            int ai_socktype;            // SOCK_STREAM, SOCK_DGRAM
            int ai_protocol;            // use 0 for "any"
            size_t ai_addrlen;          // size of ai_addr in bytes
            struct sockaddr *ai_addr;   // struct sockaddr_in or _in6
            char *ai_canonname;         // full canonical hostname
        };
    */
    //getaddrinfo() reads DNS and retrieves info of host in the form of a linked list, This info from the list can be used dynamically throughout the program
    if(status = getaddrinfo(host, port, &hints, &res) != 0){    //On success returns one or more addrinfo structures, each of which contains an Internet address that can be specified in a call to bind(2) or connect(2). Uses netdb.h (Take into account operator precedence == over =)
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status)); //gai_strerror() function specifically for errors from the getaddrinfo()
        return 2;
    }

    //struct addrinfo *res now points to a pointer that contains the information from the DNS server as a list
    //So therefore the data from res can directly be used inside a socket.(So in broad terms host than is resolved to addresses are passed into socket()) 
    int d_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(d_socket == -1){
        error("Can't open socket");
    }

    int c = connect(d_socket, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);      //Free memory used for the linked list
    if(c == -1){
        error("Can't connect to socket");
    }
return d_socket;  
}

int say(int socket, char *s){
    int result = send(socket, s, strlen(s), 0);
    if(result == -1){
        fprintf(stderr, "%s: %s\n", "Error talking to the server", strerror(errno));
    }
    return result;
}

int main(int argc, char *argv[]){

   if (argc < 3) {     //Checks if there is a argument passed
        fprintf(stderr, "Usage: %s <domain> <port>\n", argv[0]);
        exit(1);
    }

    int d_sock;

    d_sock = open_socket(argv[1], argv[2]);

    char buf[255];
    sprintf(buf, "GET / HTTP/1.1\r\n");      //sprintf() writes and stores the formatted text as a string into a character buffer in memory
    say(d_sock, buf);

    char hostRequest[255];
    sprintf(hostRequest, "Host: %s\r\n\r\n", argv[1]);  //Get the host from the first commandline arguemnt
    say(d_sock, hostRequest);
    
    char rec[256];
    int bytesRcvd = recv(d_sock, rec, 255, 0);   //Returns the number of bytes received
    while(bytesRcvd){
        if(bytesRcvd == -1){
            error("No bytes were receieved from the server");
        }

        rec[bytesRcvd] = '\0';   //Sends Null terminator to the end of the array of charaters to make it a proper string
        printf("%s", rec);
        //DO::Open a file and save the HTML, so it can be opened with a browser?
        bytesRcvd = recv(d_sock, rec, 255, 0);
    }
    close(d_sock);
    return 0;
}