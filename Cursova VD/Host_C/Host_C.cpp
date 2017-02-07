// Host_C.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
using namespace std;

#define NUM 10
int deliverBuffer[NUM];
int reciveBuffer[NUM];
char recvCharBuf[NUM];
LPTSTR NamedPipeName = L"\\\\.\\Pipe\\MyPipe";
int noError = true;
//{ 1,5,7,9,11,4,9,77,11,0}


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
	printf(" Waiting to rece datagrams from Hosts\n");
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

void OutputRecivedArray()
{
	printf(" Outputing data to recive:  ");
	for (int i = 0; i < NUM; i++)
	{
		reciveBuffer[i] = deliverBuffer[i];
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
	printf(" Host C: \n");
	printf("	Recive: NamedPipe  \n");
	printf("	Output: On screen \n");
	printf(" Press any key to start server for reciving for Host ... \n");
	_gettch();
	noError = Recive();
	if (!noError)
	{
		printf(" Press any key to exit  ... \n");
		_gettch();
		return 1;
	}
	OutputRecivedArray();
	printf(" Press any key to exit  ... \n");
	_gettch();
	return 0;

}