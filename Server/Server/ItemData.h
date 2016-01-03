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



	HWND m_hWnd;         //�����ھ��
	UINT m_id;           //���б��е�id
	CFileDlg *m_filedlg; //�����ļ�����Ի���
	CCMDShell *m_cmdshell;
	CTask *m_task;
	CMySocket m_Mysocket;//���ܣ�����ʹ��CMySocket�еĺ���,ʹ��m_sockΪ����

	//����������Ϣ
	SOCKET m_sock;       //���ܣ�����socket API �����Ĳ���
	CString m_IP;
	CString m_Address;
	//wchar_t m_Address[260];
	CString m_OS;
	CString m_Ver;
	bool Cam;
};

