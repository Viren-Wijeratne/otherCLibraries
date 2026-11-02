//This file has functions from socketFunctions.c and the function declarations from socketHeader.h
//This program is a basic http server for serving html files
//THIS PROGRAM DOES NOT WORK
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
        if(!fork()){     
            close(listener_d); //Close primary socket for the child process, as it now uses the secondary socket

            read_in(connect_d, buffer, sizeof(buffer));     //Needed to clear the HTTP request coming from the browser
            printf("%s", buffer);   
    

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

                read_in(connect_d, buffer, sizeof(buffer));     //Read in request for CSS file
                //buffer[24] = "\0";
                printf("===> Request for CSS: %s\n", buffer);
                int cssReq = strncasecmp("GET /style.css HTTP/1.1", buffer, 23);
                if(cssReq == 0){
                    FILE *fcss;
                    fcss = fopen("style.css", "r");
                    while(fgets(string, 200, fcss)){
                        send(connect_d, string, strlen(string),0);
                    } 
                }
                else if(cssReq > 0){
                        printf("This is the CSS cssReq > 0 flag\n");
                    }
                else if(cssReq < 0){
                        printf("This is the CSS cssReq < 0 flag\n");
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
        /*
        //Test to get the page linking working
        read_in(connect_d, buffer, sizeof(buffer));     //Needed to clear the HTTP request coming from the browser
        char next_page = *buffer;    
        printf("What's in the buffer chump: %c", next_page);*/
    }
    return 0;
}

/*
- The reason why this code isnt working
- The Problem: How Browsers Request Files
- In your code, you wrote your server like a linear script:
    Read the first request (which the browser sends for the HTML page).
    Send the HTML file.
    Immediately try to read again on the same socket, expecting the browser to ask for style.css.

- However, that is not how web browsers work. When your browser parses the HTML and encounters <link rel="stylesheet" href="style.css">,
    it does not wait for your server to prompt it sequentially. Instead, the browser immediately opens a brand-new, separate TCP connection 
    and sends a brand-new HTTP request: GET /style.css HTTP/1.1.

- When that second connection arrives at your server, your while(1) loop accepts it, forks a new process, and runs your main server logic. 
    But because your code is hardcoded to serve argv[1] (the HTML file) first, it treats the request for /style.css as a request for your 
    HTML page, serves the HTML file all over again (with a text/html header), and completely ignores style.css!
*/