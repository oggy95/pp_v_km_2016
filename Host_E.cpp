// Host_E.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <iostream> 

#pragma warning (disable : 4996)
#pragma comment(lib,"ws2_32.lib") //Winsock Library


#define HalfNUM 5
#define NUM 10
#define SERVER "127.0.0.1"  
#define PORT 8888
int reciveBuffer[NUM];
int deliverBuffer[NUM];
char deliverCharBuffer[NUM];
LPTSTR MailSlotName = L"\\\\.\\mailslot\\hello";
char inputFileName[] = "D:\\input_Half_For_Host_E.txt";
LPTSTR NamedPipeName = L"\\\\.\\Pipe\\MyPipe";
bool noError = true;


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

bool DeliverNamedPipe()
{
	HANDLE PipeHandle = 0;

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

bool DeliverUDP()
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

bool InputDeliverArray()
{
	try
	{
		FILE *f;
		f = fopen(inputFileName, "rt");
		for (int i = 0; i < HalfNUM; i++)
		{
			fscanf_s(f, "%d", &deliverBuffer[i]);
		}
		fclose(f);
		printf(" Integers from file:  ");
		for (int i = 0; i < HalfNUM; i++)
		{
			printf("%d ",deliverBuffer[i]);
		}
		printf("\n");
	}
	catch (char const* e)
	{
		printf("Input failed with error \n", e);
	}
	printf(" Inputing data to transfer \n");
	printf(" Input 5 integer numbesr:  ");
	for (int i = HalfNUM; i < NUM; i++)
	{
		scanf_s("%d", &deliverBuffer[i]);
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
	printf(" Host E: \n");
	printf("	Deliver #1: Mailslot  \n");
	printf("	Deliver #2: NamedPipe  \n");
	printf("	Deliver #4: UDP  \n");
	printf("	Recive: Mailsolt  \n");
	printf("	Output: On screen \n");
	printf(" Choose type of messaging:  \n");
	printf(" 0 - receive from Host A  \n");
	printf(" 1 - Mailslot  \n");
	printf(" 2 - NamedPipe  \n");
	printf(" 4 - UDP  \n");
	int a;
	scanf_s("%d",&a);
	switch (a)
	{
	case 0:
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
	case 1:
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
	case 2:
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
		DeliverNamedPipe();
		break;
	case 4:
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
		DeliverUDP();
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

