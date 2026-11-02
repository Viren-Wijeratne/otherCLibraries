//Contains all the functiond for socketTestRev5.c
#include "socketHeader.h"

//int listener_d;

void error(char *msg){
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

int read_in(int socket, char *buf, int len){
    char *s = buf;
    int slen = len;
    int c = recv(socket, s, slen, 0);
    while((c > 0) && (s[c - 1] != '\n')){
        s += c;
        slen -= c;
        c = recv(socket, s, slen, 0);
    }
    if(c < 0){
        return c;
    }
    else if(c == 0){
        buf[0] = '\0';
    }
    else{
        s[c - 1] = '\0';
    }
    return len - slen;
}

int open_listener_socket(){
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1){
        error("Can't open primary socket");
    }
    return s;
}

void bind_to_port(int socket, int port){
    struct sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_port = (in_port_t)htons(port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    //Make the port reusable, to prevent a sticky port
    int reuse = 1;
    if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1){
        error("Can't bind the port to the primary socket");
    }
    int c = bind(socket, (struct sockaddr *)&name, sizeof(name));
    if(c == -1){
        error("Cant bind to socket");
    }
}

int listen_on_port(int socket){
    if(listen(socket, 10) == -1){
        error("Can't listen on primary socket");
    }
}

int accept_connection(int socket){
    struct sockaddr_storage client_addr;
    unsigned int address_size = sizeof(client_addr);
    int connect_d = accept(socket, (struct sockaddr *)&client_addr, &address_size);
    if(connect_d == -1){
        error("Can't open the secondary socket");
    }
    return connect_d;
}

int say(int socket, char *s){
    int result = send(socket, s, strlen(s), 0);
    if(result == -1){
        fprintf(stderr, "%s: %s\n", "Error talking to the client", strerror(errno));
    }
    return result;
}
/*
void handle_shutdown(int sig){
    if(listener_d){
        close(listener_d);
    }
    fprintf(stderr, "Bye!\n");
    exit(0);
}
*/