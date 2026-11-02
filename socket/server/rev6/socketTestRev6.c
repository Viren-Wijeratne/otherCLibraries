//This file has functions from socketFunctions.c and the function declarations from socketHeader.h
//This program is a basic http server for serving html files
#include "socketHeader.h"

int main(int argc, char *argv[]){
    
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int listener_d = open_listener_socket();

    bind_to_port(listener_d, 30000);

    listen_on_port(listener_d);
  
    puts("Waiting for connection");

    char buffer[2000];

    while(1){

        int connect_d = accept_connection(listener_d);
        if(!fork()){    //When fork() returns 0 you are in the child process, so !fork() is used so here child process returns 1  
            close(listener_d); //Close primary socket for the child process, as it now uses the secondary socket

            read_in(connect_d, buffer, sizeof(buffer));     //Needed to clear the HTTP request coming from the browser
            printf("%s", buffer);   //The GET request from the client
            /*
What is happening under the hood:
1. When you point a web browser to http://localhost:30000, the browser connects and immediately sends a large HTTP request (including headers like User-Agent, Accept, etc.).
2. Your server code accepts the connection, immediately blasts the response back, and calls close(connect_d) without reading what the browser sent.
3. Because there is unread data sitting in the socket's receive buffer when close() is called, the Linux kernel sends a TCP RST (Reset) packet to the browser instead of a clean FIN/ACK shutdown.
4. The browser sees this abrupt connection reset and fails to render the page (often showing an ERR_CONNECTION_RESET or blank page error). telnet didn't care about this because it just dumps whatever raw bytes arrive, whereas a browser is strict about TCP stream lifecycles.
*/
            FILE *fptr;

            fptr = fopen(argv[1], "r");  //Open html file to serve 
            
            if(fptr != NULL){
                //Browser needs the HTTP headers !important
                char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"; //In the case of sending a php file, the argv[1] shoudl be searched if *.html send HTML header if *.php 
                send(connect_d, header, strlen(header), 0);

                char string[200];
                while(fgets(string, 200, fptr)){    //Fill the strings buffer with fptr
                    //printf("%s", string);
                    send(connect_d, string, strlen(string),0);
                }
        
                fclose(fptr);
               
            }else{
                char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
                send(connect_d, not_found, strlen(not_found), 0);
            }

            close(connect_d);   //Close the secondary socket for the child process
            exit(0);        
        }
        close(connect_d); //Close the secondary socket for the parent process
        
        //Test to get the page linking working
        read_in(connect_d, buffer, sizeof(buffer));     //Needed to clear the HTTP request coming from the browser
        char next_page = *buffer;    
        printf("What's in the buffer chump: %c", next_page);
    }
    return 0;
}

/*
* Need to add CSS file parsing
* Need to link to other html files and serve them as well
*/