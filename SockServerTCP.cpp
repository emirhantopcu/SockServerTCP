// SockServerTCP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>


#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>
#define DEFAULT_BUFLEN 512

#pragma comment(lib, "Ws2_32.lib")



using namespace std;




#define DEFAULT_PORT "5050"
#define DEFAULT_IP "10.100.60.79"

struct addrinfo* result = NULL, * ptr = NULL, hints;


void receive(SOCKET ClientSocket, char* recvbuf, int recvbuflen) {
    // Receive until the peer shuts down the connection
    int iResult, iSendResult;
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            string s = "";
            for (int i = 0; i < iResult; i++)
                s = s + recvbuf[i];


            cout << s << endl;
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();

        }
        printf("Connection closed\n");

    } while (iResult > 0);
}



int main()
{
    int iResult;
    WSADATA wsaData;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        ExitProcess(0);
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = INVALID_SOCKET;

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);




    while (true) {

        if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
            printf("Listen failed with error: %ld\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        printf("Listening...\n");

        SOCKET ClientSocket;



        ClientSocket = INVALID_SOCKET;

        char recvbuf[DEFAULT_BUFLEN];
        int recvbuflen = DEFAULT_BUFLEN;
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        else {
            printf("Connection.\n");
        }
        thread thread1(receive, ClientSocket, recvbuf, recvbuflen);
        thread1.detach();
    }

};