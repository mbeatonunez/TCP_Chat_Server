/*
 * Name:        TCP Chat Server (TCP-Client)
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


int main()
{
    //creating a socket
    int client_socket;
    if(client_socket = socket(AF_INET, SOCK_STREAM, 0) < 0){  //check for error
        perror("client: socket");
        exit(EXIT_FAILURE);
    }
    //specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    //check for connection errors
    if (connection_status == -1){
        perror("client: connect");
        exit(EXIT_FAILURE);
    }

    //receive data from server
    char server_reso

    return 0;
}
