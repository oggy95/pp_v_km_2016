// Host_D.cpp : Defines the entry point for the console application.
//
//   Отриманны данні : (сокети UDP) виводяться на екран
//   Данні для передачі: перша половина чисел з файлу 1, друга з файлу 2
//   Передає данні через: іменовані канали
//
#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
using namespace std;

#define PORT 8888 
#define SERVER "127.0.0.1"  
#define NUM 10
#define halfNUM 5
int deliverBuffer[NUM];
int reciveBuffer[NUM];
char reciveCharBuffer[NUM];
LPTSTR NamedPipeName = L"\\\\.\\Pipe\\MyPipe";
char inputFile1Name[] = "C:\\Users\\Олег\\Downloads\\Cursova VD\\input_First_Half_For_Host_D.txt";
char inputFile2Name[] = "C:\\Users\\Олег\\Downloads\\Cursova VD\\input_Second_Half_For_Host_D.txt";
bool noError = true;

bool Deliver()
{
	HANDLE PipeHandle;

	printf(" Creating NamedPipe\n");
	if (WaitNamedPipe(NamedPipeName, NMPWAIT_WAIT_FOREVER) == 0)
	{
		printf(" WaitNamedPipe failed with error %d \n", GetLastError());
		CloseHandle(PipeHandle);
		return false;
	}
	printf(" Creating data to transfer\n");
	if ((PipeHandle = CreateFile(NamedPipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL)) == INVALID_HANDLE_VALUE)
	{
		printf(" CreateFile() failed with error %d \n", GetLastError());
		CloseHandle(PipeHandle);
		return false;
	}
	printf(" Sending data to the receiver Host B\n");
	if (WriteFile(PipeHandle, deliverBuffer, sizeof(deliverBuffer), NULL, NULL) == 0)
	{
		printf(" WriteFile() failed with error %d", GetLastError());
		CloseHandle(PipeHandle);
		return false;
	}
	printf("Finished sending. Closing NamedPipe\n");
	CloseHandle(PipeHandle);
	return true;
}

bool InputDeliverArray()
{
	printf(" Inputing data to transfer \n");
	try
	{
		FILE *f;
		f = fopen(inputFile1Name, "rt");
		for (int i = 0; i < halfNUM; i++)
		{
			fscanf(f, "%d", &deliverBuffer[i]);
		}
		fclose(f);
		f = fopen(inputFile2Name, "rt");
		for (int i = halfNUM; i < NUM; i++)
		{
			fscanf(f, "%d", &deliverBuffer[i]);
		}
		fclose(f);

		printf(" Inputed data:  ");
		for (int i = 0; i < NUM; i++)
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

bool Recive()
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
	printf(" Waiting to recive datagrams from Host A\n");
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
	printf("	Recive: UDP  \n");
	printf("	Output: In file \n");
	printf("	Deliver: Namedpipe \n");
	printf("	Input: Half from file1. Half from file2  \n");
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
	printf(" !!!Before continue start Host B server  ... \n");
	printf(" Press any key to continue to deliver  ... \n");
	_gettch();
	Deliver();
	printf(" Press any key to exit  ... \n");
	_gettch();
	return 0;
}

