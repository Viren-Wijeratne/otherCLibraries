#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void error(char *msg);
int open_listener_socket();
int read_in(int socket, char *buf, int len);
void bind_to_port(int socket, int port);
int accept_connection(int socket);
int say(int socket, char *s);
void handle_shutdown(int sig);
int listen_on_port(int socket);