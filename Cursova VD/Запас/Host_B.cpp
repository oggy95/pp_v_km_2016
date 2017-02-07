// Host_B.cpp : Defines the entry point for the console application.
//
//   Отриманны данні : (іменовані канали) виводяться у файл
//   Данні для передачі: масив чисел з файлу
//   Передає данні через: сокети TCP
//

#include "stdafx.h"
#include "conio.h"
#include <stdio.h>
#include <iostream>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library


using namespace std;

#define PORT 8888
#define NUM 10
int deliverBuffer[NUM];
int reciveBuffer[NUM];
char deliverCharBuf[NUM];
char ipAdr[] = "127.0.0.1";
char inputFileName[] = "C:\\Users\\Олег\\Downloads\\Cursova VD\\input_For_Host_B.txt";
char outputFileName[] = "C:\\Users\\Олег\\Downloads\\Cursova VD\\output_For_Host_B.txt";
LPTSTR NamedPipeName = L"\\\\.\\Pipe\\MyPipe";
bool noError = true;

bool Recive()
{
	HANDLE PipeHandle;
	DWORD BytesRead;

	printf(" Creating NamedPipe\n");
	if ((PipeHandle = CreateNamedPipe(NamedPipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 1, 0, 0, 1000, NULL)) == INVALID_HANDLE_VALUE)
	{
		printf(" Create NamedPipe failed with error %d\n", GetLastError());
		CloseHandle(PipeHandle);
		return false;
	}
	printf(" Server is now running\n");
	if (ConnectNamedPipe(PipeHandle, NULL) == 0)
	{
		printf(" ConnectNamedPipe() failed with error %d\n", GetLastError());
		CloseHandle(PipeHandle);
		return false;
	}
	printf(" Waiting to rece datagrams from Host D\n");
	if (ReadFile(PipeHandle, deliverBuffer, sizeof(deliverBuffer), &BytesRead, NULL) <= 0)
	{
		printf(" ReadFile() failed with error &d", GetLastError());
		CloseHandle(PipeHandle);
		return false;
	}
	deliverBuffer[BytesRead] = 0;
	if (DisconnectNamedPipe(PipeHandle) == 0)
	{
		printf(" DisconnectNamedPipe() failed with error %d\n", GetLastError());
		CloseHandle(PipeHandle);
		return false;
	}
	printf(" Finished reciving. Closing NamedPipe.\n");
	CloseHandle(PipeHandle);
	return true;
}

bool Deliver()
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
	printf(" Ready for send data to Host C \n");


	// Some info on the receiver side...
	//getsockname(SendingSocket, (SOCKADDR *)&ServerAddr, (int *)sizeof(ServerAddr));
	//printf("Client: Receiver IP(s) used: %s\n", inet_ntoa(ServerAddr.sin_addr));
	//printf("Client: Receiver port used: %d\n", htons(ServerAddr.sin_port));

	// Sends some data to server/receiver...
	BytesSent = send(ClientSocket, deliverCharBuf, sizeof(deliverCharBuf), 0);

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

bool InputDeliverArray()
{
	printf(" Outputing data to input in file:  ");
	printf(inputFileName);
	printf("\n");
	printf(" Data taken from file:  ");
	try
	{
		FILE *f;
		f = fopen(inputFileName, "rt");
		for (int i = 0; i < NUM; i++)
		{
			fscanf(f, "%d", &deliverBuffer[i]);
		}
		fclose(f);
		for (int i = 0; i < NUM; i++)
		{
			deliverCharBuf[i] = deliverBuffer[i];
		}
		for (int i = 0; i < NUM; i++)
		{
			cout << deliverBuffer[i] << " ";
		}
		cout << endl;
		return true;
	}
	catch (char const* e)
	{
		printf(" Input failed with error \n", e);
		return false;
	}
}

bool OutputRecivedArray()
{
	printf(" Outputing data to recive in file:  ");
	printf(outputFileName);
	printf("\n");
	try
	{
		FILE *f;
		f = fopen(outputFileName, "w");

		for (int i = 0; i < NUM; i++)
		{
			fprintf(f, "%d", deliverBuffer[i]);
			fprintf(f, "\n");
		}
		fclose(f);
		return true;
	}
	catch (char const* e)
	{
		printf(" Output failed with error \n", e);
		return false;
	}
}

int main()
{
	system("color F0");
	system("mode con cols=70 lines=15");
	printf(" Host B: \n");
	printf("	Recive: Namedpipe  \n");
	printf("	Output: In file \n");
	printf("	Deliver: TCP \n");
	printf("	Input: From file  \n");
	printf(" Press any key to start server for reciving for Host D ... \n");
	_gettch();
	noError = Recive();
	if (!noError)
	{
		printf(" Press any key to exit  ... \n");
		_gettch();
		return 1;
	}
	OutputRecivedArray();
	printf(" Press any key to continue to input for deliver ... \n");
	_gettch();
	noError = InputDeliverArray();
	if (!noError)
	{
		printf(" Press any key to exit  ... \n");
		_gettch();
		return 1;
	}
	printf(" !!!Before continue start Host C server  ... \n");
	printf(" Press any key to continue to deliver  ... \n");
	_gettch();
	Deliver();
	printf(" Press any key to exit  ... \n");
	_gettch();
	return 0;
}

