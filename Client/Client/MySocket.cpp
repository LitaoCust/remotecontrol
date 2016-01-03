#include "stdafx.h"
#include "Common.h"
#include "MySocket.h"
#include <iostream>
using namespace std;


CMySocket::CMySocket()
{
}


CMySocket::~CMySocket()
{
}
SOCKET CMySocket::StartSocket(char address[60])
{
	
	cout << address << endl;
	WSADATA wsadata;
	WORD w = MAKEWORD(2, 2);
	::WSAStartup(w, &wsadata);
	SOCKET s;
	s = ::socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(address);
	addr.sin_port = htons(8050);

	if (::connect(s, (SOCKADDR*)&addr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		printf("connect error...");
		DWORD e = ::GetLastError();
		printf("  LastError:%d\n", e);
		s = NULL;
	}
	else
		printf("connect successfully\n");

	return s;
}

int CMySocket::MySend(SOCKET sock, const char* buff, int bytes)
{
	const char* b = buff;

	while (bytes > 0)
	{
		int ret = ::send(sock, b, bytes, 0);
		if (ret < 0)
		{
			printf("SOCK ERROR\n");
			return ret;
		}
		if (ret == 0)
		{
			printf("SOCK ERROR\n");
			break;
		}
		bytes -= ret;
		b += ret;
	}

	return b - (char*)buff;

}

int CMySocket::MyRecv(SOCKET sock, char* buff, int bytes)
{
	char* b = buff;
	while (bytes > 0)
	{
		int ret = ::recv(sock, b, bytes, 0);
		if (ret < 0)
		{
			return 0;
		}
		if (ret == 0)
			break;
		bytes -= ret;
		b += ret;
	}
	return b - (char*)buff;


}