#include "stdafx.h"
#include "Common.h"
#include "SystemInfo.h"


CSystemInfo::CSystemInfo()
{
}


CSystemInfo::~CSystemInfo()
{
}

/*
功能：获取操作系统版本
返回值：系统版的索引
*/
int CSystemInfo::GetSys_ver()
{
	OSVERSIONINFO osver = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&osver);
	int t;
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)
	{
		t = 0;
	}

	else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)
	{
		t = 1;
	}

	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0)
	{
		t = 2;
	}

	else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)
	{
		t = 3;
	}
	else t = 4;
	return t;
}

/*
参数：发送数据的socket
功能：发送系统信息至服务器，服务器地址在CMySocket类中定义
*/
void CSystemInfo::SendSysinfo(SOCKET sock)
{
	SYSTEMINFO_S system;
	system.os = GetSys_ver();
	system.ver = 0.1; //版本号
	system.Cam = false;
	MSGINFO_S msg;
	memset(&msg, 0, sizeof(MSGINFO_S));
	msg.Msg_id = SYSINFO;
	memcpy(msg.context, &system, sizeof(SYSTEMINFO_S));
	m_sock.MySend(sock, (char*)&msg, sizeof(MSGINFO_S));
}
