// Host_B.cpp : Defines the entry point for the console application.


#include "stdafx.h"
#include "conio.h"
#include <stdio.h>
#include <iostream>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library


using namespace std;

#define NUM 10
int deliverBuffer[NUM];
char deliverCharBuf[NUM];
char inputFileName[] = "D:\\input_For_Host_B.txt";
LPTSTR NamedPipeName = L"\\\\.\\Pipe\\MyPipe";
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
	printf(" Sending data to the receiver Host C\n");
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

int main()
{
	system("color F0");
	system("mode con cols=70 lines=15");
	printf(" Host B: \n");
	printf("	Deliver: TCP \n");
	printf("	Input: From file  \n");
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
	Deliver();
	printf(" Press any key to exit  ... \n");
	_gettch();
	return 0;
}

