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
 #include <unistd.h>
 #include <errno.h>
 #include <string.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define VERSION 1.0.5

void error(const char *msg){
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    char buffer[255];
    char name[15], newname[15];    //name up to 15 chars
    int client_socket, nameval, msgval, sd;
    int activity;                    //used with select() to check for incomming messages
    struct addrinfo address, *res, *p;

    if (argc < 3){
      fprintf(stderr, "[SYS_MSG]: usage %s hostname port\n", argv[0]);
      exit(EXIT_FAILURE);
    }

    //set of socket descriptors
    fd_set readfds, writefds;
    //clear the file descriptor socket set
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    //ask user for name
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strlen(name)-1] = '\0';    // add new name


    memset(&address, 0,sizeof(address)); //zero out server object

    //specify an address for the socket
    address.ai_family = AF_INET;
    address.ai_socktype = SOCK_STREAM;
    puts("[SYS_MSG]: Initializing......");

    int status = getaddrinfo(argv[1], argv[2] , &address, &res);
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

    //add client socket to the file descrip
    FD_SET(client_socket, &readfds);
    FD_SET(client_socket, &writefds);

    sd = client_socket;


    // //wait for server welcome message
    msgval = read(client_socket, buffer, sizeof(buffer));       //read from server
    if (msgval < 0) error("[SYS_MSG]: Error on Reading");
    printf("[SERVER]: %s\n", buffer);

    //send name to server after the connection is stablieshe
    nameval = send(client_socket, name, strlen(name), 0);   //write to server
    if (nameval < 0) error("[SYS_MSG]: Error on Writting");    //check for message error

    //start communication
    while (1){
      //if the socket descriptor is valid, then add it to the file descriptor set
      if (sd > 0) FD_SET(sd, &readfds);
      //wait for activity on the sockets indefinitely
      activity = select(sd + 1, &readfds, &writefds, NULL, NULL);
      // check for errors
      if ((activity < 0) && (errno != EINTR)) error("[SYS_MSG]: Error Select");

      if (FD_ISSET(sd, &readfds)){
          nameval = recv(sd, newname, sizeof(newname), 0);          //read name of sender
          msgval = recv(sd, buffer, sizeof(buffer), 0);
          if (msgval < 0 || nameval < 0) error("Reiev Error");

          newname[nameval] = '\0';
          buffer[msgval] = '\0';                           //add null terminator to the end of the recieved messege
          printf("[%s]: %s\n",newname, buffer);
       }
        if (FD_ISSET(sd, &writefds)){
         printf(">>> ");
         fgets(buffer, sizeof(buffer), stdin);
         send(client_socket, name, strlen(name), 0);
         send(client_socket, buffer, strlen(buffer), 0);
        }
    }

    close(client_socket);
    return 0;
}
