// Host_D.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
using namespace std;

#define PORT 8888
#define SERVER "127.0.0.1"
char ipAdr[] = "127.0.0.1";
#define NUM 10
#define halfNUM 5
int reciveBuffer[NUM];
char reciveCharBuffer[NUM];
bool noError = true;

bool OutputRecivedArray()
{
	printf(" Outputing data to recive:  ");
	for (int i = 0; i < NUM; i++)
	{
		reciveBuffer[i] = reciveCharBuffer[i];
	}
	for (int i = 0; i < NUM; i++)
	{
		cout << reciveBuffer[i] << " ";
	}
	cout << endl;
	return true;
}

bool ReciveTCP()
{
	WSADATA wsaData;
	SOCKET ListenSocket, ClientSocket;
	struct sockaddr_in server, client;
	int c;

	printf(" Initialising Winsock");
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf(" Failed. Error Code : %d", WSAGetLastError());
		// Exit with error
		return false;
	}

	printf(" Create a socket TCP\n");
	if ((ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf(" Could not create socket : %d", WSAGetLastError());
		// Do the clean up
		WSACleanup();
		// Exit with error
		return false;
	}

	printf(" Socket created.\n");

	// Prepare the sockaddr_in structure
	// IPv4
	server.sin_family = AF_INET;
	// Port no.
	server.sin_port = htons(PORT);
	// The IP address
	server.sin_addr.s_addr = inet_addr(ipAdr);;

	// Bind
	if (bind(ListenSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf(" Bind failed with error code : %d", WSAGetLastError());
		// Close the socket
		closesocket(ListenSocket);
		// Do the clean up
		WSACleanup();
		// Exit with error
		return false;
	}

	printf(" Bind done \n");

	// Listen to incoming connections
	if (listen(ListenSocket, 3) == SOCKET_ERROR) {
		printf(" Listen failed with error: %d\n", WSAGetLastError());
		// Close the socket
		closesocket(ListenSocket);
		// Do the clean up
		WSACleanup();
		// Exit with error
		return false;
	}
	printf(" Waiting for incoming connections from Host A \n");

	// Accept and incoming connection
	// Inf about client 
	c = sizeof(struct sockaddr_in);
	ClientSocket = accept(ListenSocket, (struct sockaddr *)&client, &c);
	if (ClientSocket == INVALID_SOCKET)
	{
		printf(" Accept failed with error code : %d\n", WSAGetLastError());
		// Close the socket
		closesocket(ListenSocket);
		// Do the clean up
		WSACleanup();
		// Exit with error
		return false;
	}

	printf(" Connection accepted\n");
	closesocket(ListenSocket);

	if (recv(ClientSocket, reciveCharBuffer, sizeof(reciveCharBuffer), 0) < 0)
	{
		printf(" Recv failed with error: %d\n", WSAGetLastError());
		// Close the socket
		closesocket(ClientSocket);
		// Do the clean up
		WSACleanup();
		// Exit with error
		return false;
	}
	printf(" Received from Host A!\n");
	printf(" Closing sockets\n");
	closesocket(ClientSocket);
	WSACleanup();

	return true;
}

bool ReciveUDP()
{
	int iResult = 0;
	WSADATA wsaData;

	SOCKET RecvSocket;
	sockaddr_in RecvAddr;

	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	printf(" Initialize Winsock\n");
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf(" WSAStartup failed with error %d\n", iResult);
		return false;
	}

	printf(" Create a socket for receive datagrams \n");
	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (RecvSocket == INVALID_SOCKET)
	{
		printf(" socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	printf(" Bind the socket to any address and the specified port\n");
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(PORT);
	RecvAddr.sin_addr.s_addr = inet_addr(SERVER);

	iResult = bind(RecvSocket, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
	if (iResult != 0)
	{
		printf(" bind() failed with error %d\n", WSAGetLastError());
		return false;
	}

	// Call the recvfrom function to receive datagrams
	// on the bound socket.
	printf(" Waiting to recive datagrams from Host E\n");
	iResult = recvfrom(RecvSocket, reciveCharBuffer, sizeof(reciveCharBuffer), 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);

	//std::cout << std::endl << "RecvBuf is: " << RecvBuf << std::endl << std::endl;
	//int Number = atoi(RecvBuf);
	//std::cout << std::endl << "after converting char (RecvBuf) to int (Number): " << Number << std::endl << std::endl;
	//std::cout << std::endl << "after adding 4 to the variable Number: " << Number + 4 << std::endl << std::endl;

	if (iResult == SOCKET_ERROR)
	{
		printf(" recvfrom() failed with error %d\n", WSAGetLastError());
	}

	// Close the socket when finished receiving datagrams
	printf(" Finished receiving. Closing socket.\n");
	iResult = closesocket(RecvSocket);
	if (iResult == SOCKET_ERROR) {
		printf(" closesocket() failed with error %d\n", WSAGetLastError());
		return false;
	}

	// Clean up and exit.
	printf(" Exiting\n");
	WSACleanup();
	return true;
}

int main()
{
	system("color F0");
	system("mode con cols=70 lines=15");
	printf(" Host D: \n");
	printf("	Recive 1: TCP  \n");
	printf("	Recive 2: UDP  \n");
	printf("	Output: On screen \n");
	int a;
	scanf_s("%d", &a);
	switch (a)
	{
	case 1:
		printf(" Press any key to start server for reciving for Host A ... \n");
		_gettch();
		noError = ReciveTCP();
		if (!noError)
		{
			printf(" Press any key to exit  ... \n");
			_gettch();
			return 1;
		}
		OutputRecivedArray();
		break;
	case 2:
		printf(" Press any key to start server for reciving for Host E ... \n");
		_gettch();
		noError = ReciveUDP();
		if (!noError)
		{
			printf(" Press any key to exit  ... \n");
			_gettch();
			return 1;
		}
		OutputRecivedArray();
		break;
	default:
		printf(" Press any key to exit  ... \n");
		_gettch();
		return 1;
	}
	printf(" Press any key to exit  ... \n");
	_gettch();
	return 0;
}

