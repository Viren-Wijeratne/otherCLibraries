#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int main(void)
{

	int fd;
	struct termios tty;

	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);

	if(fd == -1)
		{
			perror("[e] open");
			return 1;
		}
	printf("[+] Serial port opened.\n");

	printf("[/] Starting serial port configuration.\n");

		if(tcgetattr(fd, &tty) != 0 )
		{
			perror("[e] tcgetattr");
			close(fd);
			return 1;
		}

	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);

	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;

	tty.c_cflag |= CREAD;
	tty.c_cflag |= CLOCAL;

	tty.c_lflag = 0;
	tty.c_oflag = 0;
	tty.c_iflag = 0;

		if(tcsetattr(fd, TCSANOW, &tty) != 0)
		{
			perror("[e] tcsetattr");
			close(fd);
			return 1;
		}
	
	printf("[+] RS-485 serial port configured.\n");

	close(fd);

	printf("[-] Serial port closed.\n");

	return 0;
}
