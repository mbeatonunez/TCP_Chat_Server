/*
 * Name:        TCP Chat Server (TCP-Client)
 * Author:      Moises Beato Nunez
 * Date:        13 Mar 2018
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

#define VERSION 1.0.5

void error(const char *msg){
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    char buffer[255];
    char name[15], newname[15];    //name up to 15 chars
    int client_socket, n;
    struct addrinfo address, *res, *p; 

    if (argc < 3){
      fprintf(stderr, "[SYS_MSG]: usage %s hostname port\n", argv[0]);
      exit(EXIT_FAILURE);
    }

    //ask user for name
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strlen(name)-1] = '\0';    // add new name

    //get Server IP and port
//    server = gethostbyname(argv[1]);   //get server ip address
//    port_num = atoi(argv[2]);  //get port number

    memset(&address, '0' ,sizeof(address)); //zero out server object
    
    //specify an address for the socket
    address.ai_family = AF_INET;
    address.ai_socktype = SOCK_STREAM;
    puts("[SYS_MSG]: Initializing......");
    
    int status = getaddrinfo(argev[1], argv[2] , &address, &res);
    if (status < 0) 
        error("[SYS_MSG]: getaddrinfo fail.");
    
    // loop through all the results and connect to the first we can
    puts("[SYS_MSG]: Connection......");
    for(p = res; p != NULL; p = p->ai_next) {
        //creating a socket
        if ((client_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            error("[SYS_MSG]: Error on Socket"); //check for errors
            continue;
        }

        if (connect(client_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(client_socket);
            error("[SYS_MSG]: Error on Connection");
            continue;
        }

        break;
    } 
    
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    puts("[SYS_MSG]: Connection Established.");
    freeaddrinfo(res); // all done with this structure
    
    //wait for server welcome message
    n = read(client_socket, buffer, sizeof(buffer));       //read from server
    if (n < 0) error("[SYS_MSG]: Error on Reading");
    printf("[SERVER]: %s\n", buffer);

    //send name to server after the connection is stablieshe
    n = send(client_socket, name, strlen(name), 0);   //write to server
    if (n < 0) error("[SYS_MSG]: Error on Writting");    //check for message error

    //start communication
    while(1){
        memset(buffer, 0, sizeof(buffer));                  //clear message buffer
        printf(">>> ");
        fgets(buffer, sizeof(buffer), stdin);                 //get client message
        if(!strncmp("--q", buffer, 3)) break;                 //exit types --q
        n = send(client_socket, name, strlen(name), 0);       //write to server
        if (n < 0) error("[SYS_MSG]: Error on Writting");
        n = send(client_socket, buffer, strlen(buffer), 0);   //write to server
        if (n < 0) error("[SYS_MSG]: Error on Writting.");    //check for message error

        memset(buffer,0 , sizeof(buffer));                      //clear message from buffer
        memset(newname, 0, sizeof(newname));                      //clear name from buffer

        n = read(client_socket, newname, sizeof(newname));      //read from server
        if (n < 0) error("[SYS_MSG]: Error on Reading.");
        newname[n] = '\0';                                      //end name after last char
        n = read(client_socket, buffer, sizeof(buffer));        //read from server
        if (n < 0) error("[SYS_MSG]: Error on Reading.");
        buffer[n] = '\0';                                       //end buffer after last char
        printf("[%s]: %s\n",newname, buffer);                   //print the message and the name of the sender


    }
    close(client_socket);
    return 0;
}
