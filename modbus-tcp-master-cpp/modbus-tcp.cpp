// For Compile : g++ modbus-tcp-master.cpp -o modbus-tcp-master -lws2_32
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <winsock2.h>



#define REPLY_SIZE 65536

int main(int argc, char *argv[])
{
    int s = -1;
    struct sockaddr_in server;
    char server_reply[REPLY_SIZE] = {0};
    int recv_size = 0;
   
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("\nError: Windows socket subsytsem could not be initialized. Error Code: %d. Exiting..\n", WSAGetLastError());
        exit(1);
    }

    
    //Create a socket
    if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)    {
        printf("Error: Could not create socket: %s. Exiting..\n", strerror(errno));
        exit(1);
    }

    // Modbus TCP server address
    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Simulator
    server.sin_family = AF_INET;
    server.sin_port = htons(502); // Port numarası 502

    // Connect to server
    if (connect(s, (struct sockaddr *)(&server), sizeof(server)) < 0) {
        printf("Error: Could not connect to server: %s. Exiting..\n", strerror(errno));
        exit(1);
    }
    
    // Send HTTP request
    unsigned char array[256]={0x00,0x01,0x00,0x00,0x00,0x06,0x02,0x06,0x20,0x01,0x0d,0xac};

   
    if(send(s, (char*)array, 12, 0) < 0) {
        printf("Error: Could not send http request to server: %s. Exiting..\n", strerror(errno));
        exit(1);
    }
    
    // Receive a reply from the server
    printf("\nWaiting for server reply..\n");
    if((recv_size = recv(s, server_reply, REPLY_SIZE, 0)) < 0) {
        printf("Error: Something wrong happened while getting reply from server: %s. Exiting..\n", strerror(errno));
        exit(1);
    }

    server_reply[REPLY_SIZE - 1] = 0;

    printf("\nServer Reply:\n\n");
    for(int i=0;i<12;i++){
        printf("%0.2x:", (unsigned int)(unsigned char)server_reply[i]);
    }
    

    // Close the socket

    closesocket(s);
    WSACleanup();


    exit(0);
} // end of main