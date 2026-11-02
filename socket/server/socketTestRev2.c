/*Overall connection overview
 * (1) Bind to a port
 * (2) Listen to incoming connections
 * (3) Accept the connection
 * (4) Begin talking
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{	
	char *advice[] = 
	{
		"Take smaller bites.\r\n",
		"Go for the tight jeans. No they do NOT make you look fat.\r\n",
		"One word: Inappropriate.\r\n",
		"Just for today, be honest. tell your boss what you *really* think.\r\n",
		"You might want to rethink that haircut\r\n"
	};
//Create a socket for TCP using IPv4, with a descriptor listener_d
	int listener_d = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_port = (in_port_t)htons(30000);	//Number is converted to big endian and casted to the type in_port_t
	name.sin_addr.s_addr = htonl(INADDR_ANY);	//Accept any address
	
	int reuse = 1;	//An int variable is used to store the option, setting it to 1 means "Reuse the port"
	if(setsockopt(listener_d, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)	//Allow bind() to reuse a local address/port when old TCP connections are still in certain states
	{
		fprintf(stderr, "Can't set the reuse option on the socket");
		exit(1);
	}

	if(bind(listener_d, (struct sockaddr *) &name, sizeof(name)) == -1)		// (1)Bind the socket to port 30000 and error checking
	{	
		fprintf(stderr,"Can't Bind the port: Address already in use\n");
		exit(1);
	}	

	if(listen(listener_d, 10) == -1)	// (2) Starts to listen on port 30000, also maximum # of connections in the queue = 10 and error checking
	{
		fprintf(stderr,"Can't Listen\n");
		exit(1);
	};	
	puts("Waiting for connection");

while(1)
{
	struct sockaddr_storage client_addr;		//client_addr will store details about the client who's just connected
	unsigned int address_size = sizeof(client_addr);
	
	//This new connection descriptor (connet_d) is the one that the server will use to begin talking
	int connect_d = accept(listener_d, (struct sockaddr *) &client_addr, &address_size);	// (3) Accept the connection
	if(connect_d == -1){			
		fprintf(stderr,"Can't open secondary socket\n");
		exit(1);
	}

	char *msg = advice[rand() % 5];

	if(send(connect_d, msg, strlen(msg), 0) == -1)	// (4) Begin taliking. 
	{
		fprintf(stderr,"Error in function send\n");
		exit(1);
	}

	close(connect_d);
	}
	return 0;
}
/*Connect to the server using: telnet 127.0.0.1 30000*/
/*Socket is a 2-way datastream with both input and output so send() sys call is needed, cant use fprintf() or fscanf()*/
/*Changes from Rev1
* Added error checking for all major 4 steps
* Added setsockopt() before binding to prevent sticky ports
	- As the kernel is the one handling the network machinery, we ask the kernel to change its socket options using the setsockopt()
	- The problem in this case is actually a feature of the TCP, a state called TIME_WAIT which stays open even if the connection is 
	dropped, preventing delayed/duplicate TCP segments from an old connection from interfering with a new connection using the same 4-tuple
	- With SO_REUSEADDR enabled, certain combinations that would normally result in EADDRINUSE are permitted
*/