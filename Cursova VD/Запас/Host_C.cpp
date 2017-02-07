// Host_C.cpp : Defines the entry point for the console application.
//
//   Отриманны данні : (сокети TCP) виводяться на екран 
//   Данні для передачі: : перша половина чисел з клавіатури, друга з файлу
//   Передає данні через: майлслоти
//
#include "stdafx.h"
#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
using namespace std;

#define PORT 8888
#define NUM 10
#define HalfNUM 5
int deliverBuffer[NUM];
int reciveBuffer[NUM];
char ipAdr[] = "127.0.0.1";
char recvCharBuf[NUM];
char inputFileName[] = "C:\\Users\\Олег\\Downloads\\Cursova VD\\input_Half_For_Host_C.txt";
LPTSTR MailSlotName = L"\\\\.\\mailslot\\hello";
int noError = true;
//{ 1,5,7,9,11,4,9,77,11,0}


bool Recive()
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
	printf(" Waiting for incoming connections from Host B \n");

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

	if (recv(ClientSocket, recvCharBuf, sizeof(recvCharBuf), 0) < 0)
	{
		printf(" Recv failed with error: %d\n", WSAGetLastError());
		// Close the socket
		closesocket(ClientSocket);
		// Do the clean up
		WSACleanup();
		// Exit with error
		return false;
	}
	printf(" Received from Host B!\n");
	printf(" Closing sockets\n");
	closesocket(ClientSocket);
	WSACleanup();

	return true;
}

bool Deliver()
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

bool InputDeliverArray()
{
	printf(" Inputing data to transfer \n");
	printf(" Input 5 integer numbesr:  ");
	for (int i = 0; i < HalfNUM; i++)
	{		
		cin >> deliverBuffer[i];
	}	
	try 
	{
		FILE *f;
		f = fopen(inputFileName, "rt");
		for (int i = HalfNUM; i < NUM; i++)
		{
			fscanf(f, "%d", &deliverBuffer[i]);
		}
		fclose(f);
		printf(" Integers from file:  ");
		for (int i = HalfNUM; i < NUM; i++)
		{
			cout << deliverBuffer[i] << " ";
		}
		cout << endl;

		return true;
	}
	catch (char const* e)
	{
		printf("Input failed with error \n", e);
		return false;
	}
}

void OutputRecivedArray()
{
	printf(" Outputing data to recive:  ");
	for (int i = 0; i < NUM; i++)
	{
		reciveBuffer[i] = recvCharBuf[i];
	}
	for (int i = 0; i < NUM; i++)
	{
		cout << reciveBuffer[i] << " ";
	}
	cout << endl;
}

int main()
{
	system("color F0");
	system("mode con cols=70 lines=15");
	printf(" Host С: \n");
	printf("	Recive: TCP  \n");
	printf("	Output: On screen \n");
	printf("	Deliver: Mailslot \n");
	printf("	Input: Half from keyboard. Half from file  \n");
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