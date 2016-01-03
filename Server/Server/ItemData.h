#pragma once
#include "MySocket.h"
#include "FileDlg.h"
#include "CMDShell.h"
#include "Task.h"
#include "Common.h"
class CItemData:public CDialog
{
public:
	CItemData(UINT id, SOCKET sock, SOCKADDR_IN *addr, HWND m_hWnd);
	~CItemData();
	void Run();
	SOCKET GetSocket();
	void GetSysVer(SYSTEMINFO sys);
	static DWORD WINAPI OnListen(LPVOID lp);
	void OnBeginListen();
	void RunToFileManager();
	void RunToCmdShell();
	void RunToTask();



	HWND m_hWnd;         //主窗口句柄
	UINT m_id;           //在列表中的id
	CFileDlg *m_filedlg; //关联文件管理对话框
	CCMDShell *m_cmdshell;
	CTask *m_task;
	CMySocket m_Mysocket;//功能：用于使用CMySocket中的函数,使用m_sock为参数

	//基本数据信息
	SOCKET m_sock;       //功能：用于socket API 函数的参数
	CString m_IP;
	CString m_Address;
	//wchar_t m_Address[260];
	CString m_OS;
	CString m_Ver;
	bool Cam;
};

