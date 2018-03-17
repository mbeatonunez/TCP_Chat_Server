/*
 * Name:        TCP Chat Server (TCP-Server)
 * Author:      Moises Beato Nunez
 * Date:        16 Mar 2018
 * Description: My attempt at a TCP Chat Server meant to help me practice my C system programming. As I get better,
 *              so does the chat server. I'm using various sources and tutorials to get me started
 *
 * Operation:   Start the program from cmd line or terminal as follows:
 *                    filename  port_num (i.e. server.exe 8080)
 *
 * resources:   Learning - X YouTube channel -> https://www.youtube.com/channel/UCYtlE9Ws_5LsxTM17sK5Qnw
 *              GeeksfoGeeks article -> https://www.geeksforgeeks.org/socket-programming-cc/
 *              Rensselaer School of Science article -> http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define MAX_CLIENTS 30

#define VERSION 1.0.3           //allows for multiple connections to server via select()



void error(const char *msg){
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{

    if (argc < 2)
      fprintf(stderr, "[SYS_MSG]: Port Not Provided.\n");

    puts("[SYS_MSG]: Initializing......");

    char *message = "Welcome. You have reached the server. Type --q to quit."; //welcome message
    char buffer[255];                                               //holds the messages from client
    char client_name[15];
    int opt = 1;                                                 //option to make the server address reusable
    int server_socket, new_socket, client_socket[MAX_CLIENTS];
    int activity, incomming, clntname, i;
    int sd, max_sd;                                                  //holds socket descriptors
    struct addrinfo address, *res;
    int port_num = atoi(argv[1]);
    //set of socket descriptors
    fd_set readfds;

    memset(&address,0 ,sizeof(address)); //clear the address
    //port_num = atoi(argv[1]);                       //get port as second argument

     //specify an address for the socket
    address.ai_family = AF_INET;
    address.ai_socktype = SOCK_STREAM;
    address.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL,argv[1] , &address, &res);
    //initialize all client_socket[] to 0
     memset(client_socket,0 ,sizeof(client_socket));

    //creating a socket
    server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server_socket < 0)
        error("[SYS_MSG]: socket creation failed.");

    //set server socket to allow multiple connections
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
        error("[SYS_MSG]: setsockopt Failed.");

    //bind socket to address
    if (bind(server_socket,  res->ai_addr, res->ai_addrlen) < 0)
          error("[SYS_MSG]: Error on Binding");

    printf("[SYS_MSG]: Listening on port %d......\n", port_num);

    if(listen(server_socket, MAX_CLIENTS) < 0) //listen for connection
        error("[SYS_MSG]: Error Listning");

    //accept the incoming connections
    puts("[SYS_MSG]: Waiting for connection......");

    while (1){
        //clear the file descriptor socket set
        FD_ZERO(&readfds);

        //add server socket to the file descrip
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        //add child sockets to set
        for (i = 0; i < MAX_CLIENTS; i++){
            //socket socket descriptors
            sd = client_socket[i];

            //if the socket descriptor is valid, then add it to the file descriptor set
            if (sd > 0) FD_SET(sd, &readfds);

            //set highest file descriptor number for the select() function
            if (sd > max_sd) max_sd = sd;
        }

        //wait for activity on the sockets indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        // check for errors
        if ((activity < 0) && (errno != EINTR)) error("[SYS_MSG]: Error Select");
        //check for incomming connections
        if (FD_ISSET(server_socket, &readfds)){
            //accept an incomming connection
            new_socket = accept(server_socket, (struct sockaddr*)&address, &res->ai_addrlen);
            if (new_socket < 0) error("[SYS_MSG]: Incomming Connection Not Accepted."); //check for errors
            //print the new connection's data
            // printf("[SYS_MSG]: New Connection.\n"
            // "\tSocket FD  -> %d\n"
            // "\tIP Address -> %s\n"
            // "\tPort       -> %d\n", new_socket, , ntohs(port_num));
            //send the new connection a greeting
            if (send(new_socket, message, strlen(message), 0) != strlen(message))
                error("[SYS_MSG]: Send failed.");  //check for errors

            puts("[SYS_MSG]: Welcome message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < MAX_CLIENTS; i++){
                //if an empty spot is available
                if (client_socket[i] == 0){
                    client_socket[i] = new_socket;                          //add new socket to empty spot
                    clntname = read(new_socket, client_name, sizeof(client_name));   //get clients name after connection is established
                    client_name[clntname] = '\0';                               //add null terminator at the end of the
                    printf("[SYS_MSG]: %s has been added to the client list.\n", client_name);
                    break;
                }
            }
        }
        //else ist an IO operation
        for (i = 0; i < MAX_CLIENTS; i++){
            sd = client_socket[i];
            //check for activity in the socket
            if (FD_ISSET(sd, &readfds)){
                clntname = read(new_socket, client_name, sizeof(client_name));  //read name of sender
                incomming = read(sd, buffer, sizeof(buffer));                   //read incomming message

                if (incomming == 0){                           //check for a disconnection
                    getpeername(sd, (struct sockaddr*)&address, &res->ai_addrlen); //get socket data

                    // printf("[SYS_MSG]:Client Disconnected.\n"               //print a diconnect message
                    // "\tName       -> %s\n"
                    // "\tIP Address -> %s\n"
                    // "\tPort       -> %d\n",client_name,, ntohs(port_num));

                    close(sd); //close the socket
                    client_socket[i] = 0;  //set closed socket to 0 for reuse

                } else {
                    client_name[clntname] = '\0';
                    buffer[incomming] = '\0';                           //add null terminator to the end of the incomming messege

                    printf("[%s]: %s\n", client_name, buffer);    //print message to the screen

                    //echo message back to all clients, including the name of the message sender
                    for (int j = 0; j < MAX_CLIENTS; j++){
                        if (client_socket[j] == 0) continue; //skip empty sockets
                        else{
                            sd = client_socket[j];
                            send(sd, client_name, strlen(client_name), 0);
                            send(sd, buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }
    return 0;
}
