/*
 * Name:        TCP Chat Server (TCP-Server)
 * Author:      Moises Beato Nunez
 * Date:        07 Mar 2018
 * Description: My attempt at a TCP Chat Server meant to help me practice my C system programming. As I get better,
 *              so does the chat server. I'm using various sources and tutorials to get me started
 *
 * Operation:   Start the program from cmd line or terminal as follows:
 *                    filename  port_num (i.e. server.exe 8080)
 *
 * resources:     Learning - X YouTube channel -> https://www.youtube.com/channel/UCYtlE9Ws_5LsxTM17sK5Qnw
 *              GeeksfoGeeks article -> https://www.geeksforgeeks.org/socket-programming-cc/
 *              Rensselaer School of Science article -> http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define TRUE   1
#define FALSE  0

#define VERSION 1.0.1

void error(const char *msg){
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{

    if (argc < 2)
      fprintf(stderr, "[SYS_MSG]: Port Not Provided.\n");

    puts("[SYS_MSG]: Initializing......");
    char buffer[255];     //holds the messages between client and server

    int opt = TRUE;
    int server_socket, client_socket[5], max_client = 5;
    int port_num, n, activity, i, valread, sd;
    struct sockaddr_in address;
    socklen_t addrlen;

    //set of socket descriptors
    fd_set readfds;

    //initialize all client_socket[] to 0 so not checked
     memset(client_socket, 0 ,sizeof(client_socket));

    //creating a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
        error("[SYS_MSG]: socket creation failed.");

    //set server socket to allow multiple connections
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADD, (char *)&opt, sizeof(opt)) < 0)
        error("[SYS_MSG]: setsockopt Failed.");

    memset((char *) &address, 0 ,sizeof(address)); //clear the address
    port_num = atoi(argv[1]);                       //get port as second argument

     //specify an address for the socket
    address.sin_family = AF_INET;
    address.sin_port = htons(port_num);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
          error("[SYS_MSG]: Binding Failed.");

    puts("[SYS_MSG]: waiting for connection......");
    listen(server_socket, max_client); //listen for connection

    //accept the incoming connections
    addrlen = sizeof(address);

//    while(1){
//      bzero(buffer, sizeof(buffer));                   //clear message buffer
//      n = read(client_socket, buffer, sizeof(buffer)); //receive message
//      if (n < 0) error("[SYS_MSG]: Error on Reading.");          //check for message erors
//
//      printf("[CLIENT]: %s\n", buffer);               //printf client message
//      bzero(buffer, sizeof(buffer));                //clear message
//      fgets(buffer, sizeof(buffer), stdin);         //get server message
//
//      n = write(client_socket, buffer, strlen(buffer));  //write to client
//      if (n < 0) error("[SYS_MSG]: Error on Writting.");      //check for message erors
//
//      if(!strncmp("--q", buffer, 3)) break;       //exit if server types --q
//    }

    close(server_socket);

    return 0;
}
