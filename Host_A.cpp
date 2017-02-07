// Host_A.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
using namespace std;

#define NUM 10
char ipAdr[] = "127.0.0.1";
#define PORT 8888			
int reciveBuffer[NUM];
int deliverBuffer[NUM];
char deliverCharBuffer[NUM];
LPTSTR MailSlotName = L"\\\\.\\mailslot\\hello";
char outputFileName[] = "D:\\output_For_Host_A.txt";
bool noError = false;

bool DeliverMailSlot()
{
	HANDLE mailSlotHDL = NULL;

	// Create a mail slot named blort, allowing any size records to be received (up to 64K). Specify that we'll wait forever when reading a record.
	mailSlotHDL = CreateFile(MailSlotName, GENERIC_WRITE,
		FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (mailSlotHDL == INVALID_HANDLE_VALUE)
	{
		printf(" Mailslot created failed with error \n", GetLastError());
		CloseHandle(mailSlotHDL);
		return false;
	}

	DWORD cbWritten;

	if (WriteFile(mailSlotHDL, deliverBuffer, sizeof(deliverBuffer), &cbWritten, NULL) == 0)
	{
		printf(" Mailslot write failed with error \n", GetLastError());
		CloseHandle(mailSlotHDL);
		return false;
	}

	CloseHandle(mailSlotHDL);
	printf(" Mailslot tranfer message to Host E \n");
	return true;
}

bool DeliverTCP()
{
	WSADATA wsaData;
	SOCKET ClientSocket;
	struct sockaddr_in server, client;

	int BytesSent, nlen;

	printf("\n Initialising Winsock");
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		// Exit with error
		return false;
	}

	printf(" Create a socket TCP\n");
	if ((ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf(" Could not create socket : %d", WSAGetLastError());
		// Do the clean up
		WSACleanup();
		// Exit with error
		return false;
	}

	printf(" Socket created.\n");

	//Prepare the sockaddr_in structure
	// IPv4
	server.sin_family = AF_INET;
	// Port no.
	server.sin_port = htons(PORT);
	// The IP address
	server.sin_addr.s_addr = inet_addr(ipAdr);

	// Make a connection to the server with socket SendingSocket.
	if (connect(ClientSocket, (SOCKADDR *)&server, sizeof(server)) != 0)
	{
		printf(" connect() failed! Error code: %ld\n", WSAGetLastError());
		// Close the socket
		closesocket(ClientSocket);
		// Do the clean up
		WSACleanup();
		// Exit with error
		return false;
	}

	printf(" connect() is OK, got connected\n");
	printf(" Ready for send data to Host D \n");


	// Some info on the receiver side...
	//getsockname(SendingSocket, (SOCKADDR *)&ServerAddr, (int *)sizeof(ServerAddr));
	//printf("Client: Receiver IP(s) used: %s\n", inet_ntoa(ServerAddr.sin_addr));
	//printf("Client: Receiver port used: %d\n", htons(ServerAddr.sin_port));

	// Sends some data to server/receiver...
	BytesSent = send(ClientSocket, deliverCharBuffer, sizeof(deliverCharBuffer), 0);

	if (BytesSent == SOCKET_ERROR)
	{
		printf(" send() error %ld\n", WSAGetLastError());
		// Close the socket
		closesocket(ClientSocket);
		// Do the clean up
		WSACleanup();
		// Exit with error
		return false;
	}

	printf(" send() is OK - bytes sent: %ld\n", BytesSent);
	// Some info on this sender side...
	// Allocate the required resources
	memset(&client, 0, sizeof(client));
	nlen = sizeof(client);

	//getsockname(ClientSocket, (SOCKADDR *)&client, &nlen);
	//printf(" Sender IP(s) used: %s\n", inet_ntoa(client.sin_addr));
	//printf(" Sender port used: %d\n", htons(client.sin_port));

	printf(" Closing sockets \n");
	closesocket(ClientSocket);
	WSACleanup();

	return true;
}

bool Recive()
{
	HANDLE mailSlotHDL = NULL;

	// Create a mail slot named blort, allowing any size records to be received (up to 64K). Specify that we'll wait forever when reading a record.
	mailSlotHDL = CreateMailslot(MailSlotName, 0, MAILSLOT_WAIT_FOREVER, NULL);
	if (mailSlotHDL == INVALID_HANDLE_VALUE)
	{
		printf("\n Mailslot created failed with error \n", GetLastError());
		CloseHandle(mailSlotHDL);
		return false;
	}
	printf(" Mailslot was created ! \n");

	DWORD nBytesRead;

	printf(" Waiting to resive message from Host C \n");
	if (ReadFile(mailSlotHDL, reciveBuffer, sizeof(reciveBuffer), &nBytesRead, NULL) == 0)
	{
		printf(" Mailslot read failed with error \n", GetLastError());
		CloseHandle(mailSlotHDL);
		return false;
	}
	printf(" Read confirm \n");
	printf(" Received %d bytes from Host C:\n", nBytesRead);
	reciveBuffer[nBytesRead] = 0;
	CloseHandle(mailSlotHDL);
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

void OutputRecivedArray()
{
	printf(" Outputing data to recive:  ");
	for (int i = 0; i < NUM; i++)
	{
		std::cout << reciveBuffer[i] << " ";
	}
	std::cout << std::endl;
	printf(" Outputing data to recive in file:  ");
	printf(outputFileName);
	printf("\n");
	try
	{
		FILE *f;
		f = fopen(outputFileName, "w");

		for (int i = 0; i < NUM; i++)
		{
			fprintf(f, "%d", reciveBuffer[i]);
			fprintf(f, "\n");
		}
		fclose(f);
		
	}
	catch (char const* e)
	{
		printf(" Output failed with error \n", e);
	}
}

int main()
{
	system("color F0");
	system("mode con cols=70 lines=15");
	printf(" Host A: \n");
	printf("	Deliver: Mailslot \n");
	printf("	Input: From keyboard \n");
	printf("	Receive: To screen and file \n");
	printf("	Deliver or receive info? \n");
	printf("	0 - Deliver Mailslot \n");
	printf("	1 - Receive Mailslot \n");
	printf("	3 - Deliver TCP \n");
	int a;
	scanf_s("%d", &a);
	switch (a)
	{
	case 0:
		printf(" Press any key to continue to input for deliver ... \n");
		_gettch();
		noError = InputDeliverArray();
		if (!noError)
		{
			printf(" Press any key to exit  ... \n");
			_gettch();
			return 1;
		}
		printf(" Press any key to continue to deliver  ... \n");
		_gettch();
		DeliverMailSlot();
		break;
	case 1:
		printf(" Press any key to start server for reciving for Host A ... \n");
		_gettch();
		noError = Recive();
		if (!noError)
		{
			printf(" Press any key to exit  ... \n");
			_gettch();
			return 1;
		}
		OutputRecivedArray();
		break;
	case 3:
		printf(" Press any key to continue to input for deliver ... \n");
		_gettch();
		noError = InputDeliverArray();
		if (!noError)
		{
			printf(" Press any key to exit  ... \n");
			_gettch();
			return 1;
		}
		printf(" Press any key to continue to deliver  ... \n");
		_gettch();
		DeliverTCP();
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