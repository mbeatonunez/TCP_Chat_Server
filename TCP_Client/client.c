/*
 * Name:        TCP Chat Server (TCP-Client)
 * Author:      Moises Beato Nunez
 * Date:        07 Mar 2018
 * Description: My attempt at a TCP Chat Server meant to help me practice my C system programming. As I get better,
 *              so does the chat server. I'm using various sources and tutorials to get me started
 *
 * Operation:   Start the program from cmd line or terminal as follows:
 *                    filename server_ip port_num (i.e. client.exe 10.10.187.23 8080)
 *
 * resources:   Learning - X YouTube channel -> https://www.youtube.com/channel/UCYtlE9Ws_5LsxTM17sK5Qnw
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
#include <netdb.h>

#define VERSION 1.0.1

void error(const char *msg){
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    char buffer[255];
    int client_socket, port_num, n;
    struct sockaddr_in server_address;  //create server object
    struct hostent *server;

    if (argc < 3){
      fprintf(stderr, "[SYS_MSG]: usage %s hostname port\n", argv[0]);
      exit(EXIT_FAILURE);
    }

    //get Server IP and port
    server = gethostbyname(argv[1]);   //get server ip address
    port_num = atoi(argv[2]);  //get port number


    puts("[SYS_MSG]: Initializing......");

    //creating a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) error("[SYS_MSG]: Error opening socket"); //check for errors


    if (server == NULL) fprintf(stderr, "[SYS_MSG]: Invalid Server Address\n");

    memset((char *) &server_address, 0 ,sizeof(server_address)); //zero out server object

    //specify an address for the socket
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_num);
    bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, server->h_length);

    puts("[SYS_MSG]: Connection......");
    int connection_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    //check for connection errors
    if (connection_status == -1)
      error("[SYS_MSG]: connection failed to stablish.");

    puts("[SYS_MSG]: Connection Established.");
    //start communication
    while(1){
      memset(buffer, 0, sizeof(buffer));                    //clear message buffer
      fgets(buffer, sizeof(buffer), stdin);             //get server message
      n = write(client_socket, buffer, strlen(buffer));  //write to client
      if (n < 0) error("[SYS_MSG]: Error on Writting.");      //check for message erors

      memset(buffer, 0, sizeof(buffer));
      n = read(client_socket, buffer, sizeof(buffer)); //receive message
      if (n < 0) error("[SYS_MSG]: Error on Reading.");

      printf("[SERVER]: %s\n", buffer);

      if(!strncmp("--q", buffer, 3)) break;       //exit if server types --q
    }
    close(client_socket);
    return 0;
}
