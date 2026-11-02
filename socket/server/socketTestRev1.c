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
	
	bind(listener_d, (struct sockaddr *) &name, sizeof(name));	// (1)Bind the socket to port 30000

	listen(listener_d, 10);		// (2) Starts to listen on port 30000, also maximum # of connections in the queue = 10
	puts("Waiting for connection");
while(1){
	struct sockaddr_storage client_addr;		//client_addr will store details about the client who's just connected
	unsigned int address_size = sizeof(client_addr);
	
	//This new connection descriptor (connet_d) is the one that the server will use to begin talking
	int connect_d = accept(listener_d, (struct sockaddr *) &client_addr, &address_size);	// (3) Accept the connection

	char *msg = advice[rand() % 5];

	send(connect_d, msg, strlen(msg), 0);	// (4) Begin taliking. Socket is a 2-way datastream with both input and output so send() sys call is needed, cant use fprintf() or fscanf()
	close(connect_d);
	}
	return 0;
}
/*Connect to the server using: telnet 127.0.0.1 30000*/
/*The problem with this program is that the bound port is sticky and if the server restarts in a short period the client cannot connect to it afterwards */