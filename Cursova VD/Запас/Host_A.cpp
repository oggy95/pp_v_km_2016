// Host_A.cpp : Defines the entry point for the console application.
//
//   Данні для передачі вводяться массивом з клавіатури
//   Передає данні через UDP сокети
//
#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
using namespace std;

#define NUM 10
#define SERVER "127.0.0.1"  
#define PORT 8888			
int deliverBuffer[NUM];
char deliverCharBuffer[NUM];
bool noError = false;

bool Deliver()
{
	int iResult = 0;
	WSADATA wsaData;

	SOCKET SendSocket = INVALID_SOCKET;
	sockaddr_in RecvAddr;

	printf(" Initialize Winsock\n");
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) 
	{
		printf(" WSAStartup failed with error: %d\n", iResult);
		return false;
	}

	printf(" Create a socket for sending datagrams\n");
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET) 
	{
		printf(" socket() failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	// Set up the RecvAddr structure with the IP address of
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(PORT);
	RecvAddr.sin_addr.s_addr = inet_addr(SERVER);

	// Send a datagram to the receiver
	printf(" Sending to the receiver Host D\n");
	iResult = sendto(SendSocket, deliverCharBuffer, sizeof(deliverCharBuffer), 0, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
	if (iResult == SOCKET_ERROR)
	{
		printf(" sendto() failed with error: %d\n", WSAGetLastError());
		closesocket(SendSocket);
		WSACleanup();
		return false;
	}

	// When the application is finished sending, close the socket.
	printf(" Finished sending. Closing socket.\n");
	iResult = closesocket(SendSocket);
	if (iResult == SOCKET_ERROR) 
	{
		printf(" closesocket() failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	// Clean up and quit.
	printf(" Exiting\n");
	WSACleanup();
	return true;
}

bool InputDeliverArray()
{
	printf(" Inputing data to transfer \n");
	printf(" Input 10 integer numbesr:   ");
	for (int i = 0; i < NUM; i++)
	{
		cin >> deliverBuffer[i];
	}
	for (int i = 0; i < NUM; i++)
	{
		deliverCharBuffer[i] = deliverBuffer[i];
	}
	return true;
}

int main()
{
	system("color F0");
	system("mode con cols=70 lines=15");
	printf(" Host A: \n");
	printf("	Deliver: UDP \n");
	printf("	Input: From keyboard \n");
	printf(" Press any key to continue to input for deliver ... \n");
	_gettch();
	noError = InputDeliverArray();
	if (!noError)
	{
		printf(" Press any key to exit  ... \n");
		_gettch();
		return 1;
	}
	printf(" !!!Before continue start Host B server  ... \n");
	printf(" Press any key to continue to deliver  ... \n");
	_gettch();
	Deliver();
	printf(" Press any key to exit  ... \n");
	_gettch();
	return 0;
}