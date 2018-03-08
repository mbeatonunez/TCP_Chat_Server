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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define VERSION 1.0
#define MAXPENDING 5

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

    int server_socket, client_socket, port_num, n;
    struct sockaddr_in server_address, client_address;
    socklen_t client_addr_len;

    //creating a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
        error("[SYS_MSG]: socket creation failed.");

    bzero((char *) &server_address, sizeof(server_address)); //clear the address
    port_num = atoi(argv[1]);           //get port as second argument

     //specify an address for the socket
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_num);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
          error("[SYS_MSG]: Binding Failed.");

    puts("[SYS_MSG]: waiting for connection......");
    listen(server_socket, MAXPENDING); //listen for connection

    client_addr_len = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_addr_len);  //accept a connection
    if (client_socket < 0)
        perror("[SYS_MSG]: Connection failed.");

    puts("[SYS_MSG]: Connection Established.");
    while(1){
      bzero(buffer, sizeof(buffer));                   //clear message buffer
      n = read(client_socket, buffer, sizeof(buffer)); //receive message
      if (n < 0) error("[SYS_MSG]: Error on Reading.");          //check for message erors

      printf("[CLIENT]: %s\n", buffer);               //printf client message
      bzero(buffer, sizeof(buffer));                //clear message
      fgets(buffer, sizeof(buffer), stdin);         //get server message

      n = write(client_socket, buffer, strlen(buffer));  //write to client
      if (n < 0) error("[SYS_MSG]: Error on Writting.");      //check for message erors

      if(!strncmp("--q", buffer, 3)) break;       //exit if server types --q
    }
    close(client_socket);
    close(server_socket);

    return 0;
}
