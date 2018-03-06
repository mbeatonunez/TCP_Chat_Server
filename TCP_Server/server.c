/*
 * Name:        TCP Chat Server (TCP-Server)
 * Author:      Moises Beato Nunez
 * Date:        04 Mar 2018
 * Description: My attempt at a TCP Chat Server meant to help me practice my C system programming. As I get better,
 *              so does the chat server. I'm using various sources and tutorials to get me started
 *
 * sources:     Learning - X YouTube channel -> https://www.youtube.com/channel/UCYtlE9Ws_5LsxTM17sK5Qnw
 *              GeeksfoGeeks article -> https://www.geeksforgeeks.org/socket-programming-cc/
 *              Rensselaer School of Science article -> http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/type.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXPENDING 5

int main(int argc, char **argv)
{
    puts("[SYS_MSG]: Initializing......");
    char server_message[256] = "Connection Stablished." 
    
    //creating a socket
    int server_socket, client_socket;
    if(server_socket = socket(AF_INET, SOCK_STREAM, 0) < 0){  //check for error
        perror("[SYS_MSG]: socket creation failed.");
        exit(EXIT_FAILURE);
    }
     //specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        perror("[SYS_MSG]: Binding Failed.");
        exit(EXIT_FAILURE);
    }
    puts("[SYS_MSG]: waiting for connection......");
    if (listen(server_socket, MAXPENDING) < 0) {
        perror("[SYS_MSG]: Listening Failed.");
        exit(EXIT_FAILURE);
    }
    
    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0){
        perror("[SYS_MSG]: Connection failed.");
        exit(EXIT_FAILURE);
    }
    
    send
	return 0;
}
