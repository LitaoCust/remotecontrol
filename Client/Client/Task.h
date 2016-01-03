#pragma once
#include "MySocket.h"
class CTask
{
public:
	CTask();
	~CTask();
	void SendTask(SOCKET sock);
	void KillTask(DWORD KillID, SOCKET sock);
	BOOL EnablePrivilege(HANDLE hToken, LPCWSTR szPrivName);
private:
	CMySocket m_sock;

};


