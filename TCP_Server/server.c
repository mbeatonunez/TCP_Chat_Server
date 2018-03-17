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
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define MAX_CLIENTS 30

#define VERSION 1.0.4           //allows for multiple connections to server via select()



void error(const char *msg){
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{


    char *message = "Welcome. You have reached the server. Type --q to quit.";  //welcome message
    char buffer[255];                //holds the messages from client
    char client_name[15];            //hold the name of the client  
    
    int opt = 1;                     //option to make the server address reusable
    int server_socket, new_socket, client_socket[MAX_CLIENTS]; //listening socket, incomming socket and a array of the connected sockets
    int activity;                    //used with select() to check for incomming connection or messages
    int msgval;                      //hold the number of chars of the incomming massave
    int nameval;                     //hold the value of sender's name
    int i;          
    int sd, max_sd;                  //holds socket descriptors
    
    struct addrinfo address, *res, *p; //server address information
    struct sockaddr_storage clnt_addr; //connection address information
    socklen_t addr_size;               //size of the connection address
   
    //set of socket descriptors
    fd_set readfds;
    
    //check main arguments
    if (argc < 2)
      fprintf(stderr, "[SYS_MSG]: Port Not Provided.\n"); exit(1);
    
    int port_num = argv[1];  //port number to listen on

    puts("[SYS_MSG]: Initializing......");

    memset(&address,0 ,sizeof(address)); //clear the address
  
    //specify an address for the socket
    address.ai_family = AF_INET;            //use IPV4
    address.ai_socktype = SOCK_STREAM;      //use a TCP connection
    address.ai_flags = AI_PASSIVE;          //use the IP address of the machine running the program
    
    //get the ip address info.
    int status = getaddrinfo(NULL, port_num , &address, &res); 
    if (status < 0) 
        error("[SYS_MSG]: getaddrinfo fail.");
        
    //initialize all client_socket[] to 0
     memset(client_socket,0 ,sizeof(client_socket));
    
    // loop through all the results and bind to the first we can
    for(p = res; p != NULL; p = p->ai_next) {
        //creating a socket
        if ((server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("[SYS_MSG]: socket creation failed.");
            continue;
        }
        //set server socket to allow multiple connections
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
            error("[SYS_MSG]: setsockopt Failed.");
        
        //bind socket to address
        if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(server_socket);
            perror("[SYS_MSG]: bind");
            continue;
        }
        break;
    }
    // all done with this structure
    freeaddrinfo(res);
    
    //if bind fails
    if (p == NULL) 
    error("[SYS_MSG]: Failed to Bind.");
 
    printf("[SYS_MSG]: Listening on port %d......\n", port_num);
    
    //listen for connection
    if(listen(server_socket, MAX_CLIENTS) < 0) 
        error("[SYS_MSG]: Error Listning");

    //accept the incoming connections
    puts("[SYS_MSG]: Waiting for connection......");
    
    //set the size of the incomming address
    addr_size = sizeof clnt_addr;
    
    while (1){  //do forever
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
            new_socket = accept(server_socket, (struct sockaddr *)&clnt_addr, &addr_size);
            if (new_socket < 0) error("[SYS_MSG]: Incomming Connection Not Accepted."); //check for errors
            //print the new connection's data
            // printf("[SYS_MSG]: New Connection.\n"
            // "\tSocket FD  -> %d\n"
            // "\tIP Address -> %s\n"
            // "\tPort       -> %d\n", new_socket, inet_ntop(clnt_addr), port_num);
            //send the new connection a greeting
            if (send(new_socket, message, strlen(message), 0) != strlen(message))
                error("[SYS_MSG]: Send failed.");  //check for errors

            puts("[SYS_MSG]: Welcome message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < MAX_CLIENTS; i++){
                //if an empty spot is available
                if (client_socket[i] == 0){
                    client_socket[i] = new_socket;                          //add new socket to empty spot
                    nameval = recv(new_socket, client_name, sizeof(client_name), 0);   //get clients name after connection is established
                    client_name[nameval] = '\0';                               //print the new name to confirm it was recieved
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
                nameval = recv(sd, client_name, sizeof(client_name), 0);          //read name of sender
                msgval = recv(sd, buffer, sizeof(buffer), 0);                   //read incomming message

                if (msgval == 0){                           //check for a disconnection
                    getpeername(sd, (struct sockaddr *)&clnt_addr, &addr_size); //get socket data

                    // printf("[SYS_MSG]:Client Disconnected.\n"               //print a diconnect message
                    // "\tName       -> %s\n"
                    // "\tIP Address -> %s\n"
                    // "\tPort       -> %d\n",client_name, inet_ntop(clnt_addr), port_num);
                    
                    FD_CLR(sd, &readfds); // remove from master set
                    close(sd); //close the socket
                    client_socket[i] = 0;   

                } else {
                    client_name[nameval] = '\0';
                    buffer[msgval] = '\0';                           //add null terminator to the end of the recieved messege

                    printf("[%s]: %s\n", client_name, buffer);    //print message to the screen

                    //echo message back to all clients, including the name of the message sender
                    for (int j = 0; j < MAX_CLIENTS; j++){
                        if (client_socket[j] == 0) continue; //skip empty or closed sockets
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
