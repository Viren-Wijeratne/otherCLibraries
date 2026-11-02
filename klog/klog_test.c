//This is a test program to run klogctl to get messages from the kernel ring buffer
//Be AWARE this program reads the ring buffer recursively so it forms an infinit loop

#include <stdio.h>
#include <sys/klog.h>

int main(){
    while(1){
        char buffer[20000];
        //SYSLOG_ACTION_READ_ALL (3)
        int status = klogctl(3, buffer, sizeof(buffer));

        if(status == -1){
            perror("klogctl failed to receive the ring buffer");
            return 1;
        }

        if(status > 0){
            size_t written = fwrite(buffer, 1, (size_t)status, stdout);
            if (written != (size_t)status) {
                perror("fwrite");
                return 1;
            }            
        }
    }
}

/*To be added, 
- read the kernel log once, append any new kernel messages to the bottom
- remove the <6> part of the log -> maybe split the message into two halfs using sscanf
    Format of the output:
        eg: <6>[17849.244096] usb 1-1: new full-speed USB device number 8 using xhci_hcd ==> <log_level>[index] message
    
*/