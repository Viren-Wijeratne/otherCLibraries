//The system() function can run a normal commandline program, returns 0 if successful, unsuccessful non-zero (check: man system)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char buffer[50];

    //strcpy(buffer, "cmatrix");
    strcpy(buffer, "cmatrix");
    int flag = 0;

    //Runs the command on the buffer, no need for printf()
    while(flag == 0){
        flag = system(buffer);
    }

    return 0;
}