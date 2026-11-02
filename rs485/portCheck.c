#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void){

int fd;

fd = open("/dev/ttyUSB0", O_RDWR);

	if(fd == -1){
		perror("open");
		return 1;
	}	

printf("[+] Serial port opened.\n");

close(fd);

printf("[-] Serial port closed.\n");

return 0;
}
