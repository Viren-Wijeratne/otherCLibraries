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
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int listener_d;

//Funcion declarations
void error(char *msg);
int open_listener_socket();
int read_in(int socket, char *buf, int len);
void bind_to_port(int socket, int port);
int accept_connection(int socket);
int say(int socket, char *s);
void handle_shutdown(int sig);
int listen_on_port(int socket);

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
	int listener_d = open_listener_socket();

	bind_to_port(listener_d, 30000);		// (1)Bind the socket to port 30000 and error checking

	listen_on_port(listener_d);			// (2) Starts to listen on port 30000, also maximum # of connections in the queue = 10 and error checking

	puts("Waiting for connection");

while(1)
{
	int connect_d  = accept_connection(listener_d);	// (3) Accept the connection

	say(connect_d, advice[rand() % 5]);		// (4) Begin taliking.
	close(connect_d);
	
}

	return 0;
}

//Function definitions
void error(char *msg){
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
}

int read_in(int socket, char *buf, int len){
	char *s;
	int slen = len;
	int c = recv(socket, s, slen, 0);
	while((c > 0) && (s[c - 1] != '\n')){
		s += c;
		slen -= c;
		c = recv(socket, s, slen, 0);
	}

	if(c < 0){
		return c;
	}
	else if(c == 0){
		buf[0] = '\0';
	}
	else{
		s[c -1] = '\0';
	}
	return len - slen;
}

int open_listener_socket(){
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if(s == -1){
		error("Cant open socket");
	}
	return s;
}

void bind_to_port(int socket, int port){
	struct sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_port = (in_port_t)htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	int reuse = 1;
	if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1){
		error("Can't set the reuse option on the socket");
	}
	int c = bind(socket, (struct sockaddr *) &name, sizeof(name));
	if(c == -1){
		error("Can't bind to socket");
	}
}
int listen_on_port(int socket){
	if(listen(socket, 10) == -1)
	{
		error("Can't Listen");
	};
}

int accept_connection(int socket){
	struct sockaddr_storage client_addr;		//client_addr will store details about the client who's just connected
	unsigned int address_size = sizeof(client_addr);
	int connect_d = accept(socket, (struct sockaddr *) &client_addr, &address_size);
	if(connect_d == -1){			
		error("Can't open secondary socket");
	}
	return connect_d;
}

int say(int socket, char *s){
	int result = send(socket, s, strlen(s), 0);
	if(result = -1){
		fprintf(stderr, "%s: %s\n", "Error talking to the client", strerror(errno));
	}
	return result;
}

void handle_shutdown(int sig){
	if(listener_d){
		close(listener_d);
	}
	fprintf(stderr, "Bye!\n");
	exit(0);
}

/*Connect to the server using: telnet 127.0.0.1 30000*/

/*Changes from Rev2
* New function to show an error message => error()
* New function to read received data from client => read_in()
* New function to open an IPv4 socket => open_listener_socket()
* New function to bind to a port => bind_to_port() 
* New function to listen on the port for incoming connections => listen_on_port()
* New function to accept new connection on the port => accept_connection()
* New function to send a string to a client => say()
* New function to close the listener_d => handle_shutdown()
* Replaced previouse code with appropriate functions
*/