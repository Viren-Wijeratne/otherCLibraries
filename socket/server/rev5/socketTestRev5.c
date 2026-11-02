//This file has functions from socketFunctions.c and the function declarations from socketHeader.h
#include "socketHeader.h"

int main(){

    int listener_d = open_listener_socket();

    bind_to_port(listener_d, 30000);

    listen_on_port(listener_d);

    puts("Waiting for connection");

    char buffer[255];

    while(1){

        int connect_d = accept_connection(listener_d);
        if(!fork()){    //When fork() returns 0 you are in the child process, so !fork() is used so here child process returns 1  
                close(listener_d); //Close primary socket for the child process, as it now uses the secondary socket
                if(say(connect_d, "Internet Knock-Knock Protocol Server\r\n Version 1.0\r\nKnock-Knock\r\n") != -1){
                    read_in(connect_d, buffer,sizeof(buffer));
                    if(strncasecmp("Who's there?", buffer, 12)){
                        say(connect_d, "You you say 'Who's there?'");
                    }else{
                        if(say(connect_d, "Oscar\r\n") != -1){
                            read_in(connect_d, buffer, sizeof(buffer));
                            if(strncasecmp("Oscar who?", buffer, 10)){
                                say(connect_d, "You should say 'Oscar who?'\r\n");
                            }else{
                                say(connect_d, "Oscar silly question you get a silly answer\r\n");
                            }
                        }
                    }
                }
            close(connect_d);   //Close the secondary socket for the child process after everything
            exit(0);        
        }
        close(connect_d); //Close the secondary socket for the parent process
    }
    return 0;
}
/* 
The primary socket can keep listening anf accepting new connections as long as the new connection moves to a child process with a secondary socket
and frees up the primary socket. If this is not done incoming connection after the first request will be put on a queue and will be server one
after the other. Th number of connection that can be accommodate by the queue is defined in listen()
*/