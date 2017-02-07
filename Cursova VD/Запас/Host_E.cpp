// Host_E.cpp : Defines the entry point for the console application.
//
//   Отриманны данні : (майлслоти) виводяться на екран 
//
#include "stdafx.h"
#include <windows.h>
#include <iostream> 


#define NUM 10
int reciveBuffer[NUM];
LPTSTR MailSlotName = L"\\\\.\\mailslot\\hello";
bool noError = true;

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

void OutputRecivedArray()
{
	printf(" Outputing data to recive:  ");
	for (int i = 0; i < NUM; i++)
	{
		std::cout << reciveBuffer[i] << " ";
	}
	std::cout << std::endl;
}

int main()
{
	system("color F0");
	system("mode con cols=70 lines=15");
	printf(" Host E: \n");
	printf("	Recive: Mailsolt  \n");
	printf("	Output: On screen \n");
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
	printf(" Press any key to exit  ... \n");
	_gettch();
	return 0;
}

