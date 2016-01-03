#pragma once
#include "MySocket.h"
class CSystemInfo
{
public:
	CSystemInfo();
	~CSystemInfo();
	void SendSysinfo(SOCKET sock);
	CMySocket m_sock;
private:
	int GetSys_ver();
};

