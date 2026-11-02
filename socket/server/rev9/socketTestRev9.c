//This file has functions from socketFunctions.c and the function declarations from socketHeader.h
//This program is a basic http server for serving html files and css files

#include "socketHeader.h"
#include <stdbool.h>

bool search_files(char *file);
const char *relavent_content_type(const char *path);

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

            //char *filelookup = path;
            bool filefound = search_files(path);       //returns 0 when successfull, 1 when unsuccessful

            //Check if the requested file is in the same file path as the source file
            if(filefound == true){
                printf("File %s found!\n", path);
                char filepath[300]; //Because path returns /index.html not ./index.html
                snprintf(filepath, sizeof(filepath),".%s", path );

                FILE *fptr = fopen(filepath, "r");
                

                //Update the content_type according to the file requested by the client
                const char *content_type = relavent_content_type(path);

                if(fptr != NULL){
                char header[256];
                snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type:%s\r\n\r\n", content_type);
                send(connect_d, header, strlen(header), 0);
            }
        
                //Send the content of the file pointered to by the file descriptor recursively
                char string[200];
                if(fptr != NULL){
                    while(fgets(string, 200, fptr)){    //Fill the strings buffer with fptr
                        send(connect_d, string, strlen(string),0);
                    }
                }
            fclose(fptr);            
            }
            else{
                printf("%s: Requested file could not be found\n", path);
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

bool search_files(char *file){
    char filename[1024]; 
    snprintf(filename, sizeof(filename),".%s", file);
    FILE *fptrtemp = fopen(filename, "r");
    if(fptrtemp == NULL){
        return false;
    }else if(fptrtemp > 0){
        fclose(fptrtemp);
        return true;
    }else if(fptrtemp < 0){
        printf("Problem with function search_files()");
        return false;
    }
}

const char *relavent_content_type(const char *path){
    const char *ext = strrchr(path, '.');   //strrchr() returns pointer from the character, strchr() returns pointer before the character; man :-function returns a pointer to the last occurrence of the character c
    //printf("ext from the relavent_content_type() is: %s\n", ext);
    if(ext == NULL){
        return "text/plain;charset=UTF-8";
    }
    if(strcasecmp(ext, ".html") == 0 || strcasecmp(ext, ".htm") == 0){
        //printf("ran html line for content type\n");
        return "text/html; charset=UTF-8";
    }
    if(strcasecmp(ext, ".css") == 0){
        //printf("ran css line for content type\n");
        return "text/css; charset=UTF-8";
    }
    if(strcasecmp(ext, ".js") == 0){
        //printf("ran js line for content type\n");
        return "application/javascript";
    }
    //printf("ran with default line for content type\n");
    return "text/plain";
}

/*Todo
- Add image delivery
- Solve the font size problem in the browser
- Add PHP support?

*/