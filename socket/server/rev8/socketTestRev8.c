//This file has functions from socketFunctions.c and the function declarations from socketHeader.h
//This program is a basic http server for serving html files and css files

#include "socketHeader.h"

int main(int argc, char *argv[]){

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
            char method[16];
            char path[255];
            sscanf(buffer, "%15s %254s", method, path);     //Parse the initial browser request for the webpage
            printf("The browser has requested: %s\n", path);
            
            if(argv[1] == NULL){
                argv[1] = "index.html";
            }

            FILE *fptr = NULL;
            char *content_type = "type/plain";
            
            //Implement a search algorithm to go through the files in the hosting folder, so any request from the browser can be loaded and served dynamically
            if(strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0){
                 fptr = fopen(argv[1], "r");  //Open html file to serve 
                 content_type = "text/html; charset=UTF-8";
            }
            //Maybe setup an enum for a switch statement for possible files requested by browser. eg: strcmp(path, enum page) == 0. Kinda redundant if the search is used though
            else if(strcmp(path, "/style.css") == 0){
                fptr = fopen("style.css", "r");
                content_type = "text/css; charset=UTF-8";
            }
            else if(strcmp(path, "/script.js") == 0){
                fptr = fopen("script.js", "r");
                content_type = "application/javascript";
            }
            //Implement the favicon request -> response
            else if(strcmp(path, "/favicon.png") == 0){
                fptr = fopen("/favicon.png", "r");
                content_type = "image/png";
            }


            if(fptr != NULL){
                char header[256];
                snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type:%s\r\n\r\n", content_type);
                send(connect_d, header, strlen(header), 0);
            
                char string[200];
                while(fgets(string, 200, fptr)){    //Fill the strings buffer with fptr
                    send(connect_d, string, strlen(string),0);
                }
            fclose(fptr);            
            }

            else{
                char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
                send(connect_d, not_found, strlen(not_found), 0);
            }

            close(connect_d);   //Close the secondary socket for the child process
            exit(0);        
        }
        close(connect_d); //Close the secondary socket for the parent process
       
    }
    return 0;
}
