#pragma once
class CMySocket
{
public:
	CMySocket();
	~CMySocket();
	int SendCommand(SOCKET socket, const char* buf, int bytes);
	int RecvCommand(SOCKET socket, char* buf, int bytes);
	void Clean();
};

