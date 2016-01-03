#include "stdafx.h"
#include "Task.h"
#include <tlhelp32.h>
#include <Psapi.h>
#include "Common.h"
#pragma comment(lib,"Psapi.lib")

CTask::CTask()
{
}


CTask::~CTask()
{
}
BOOL CTask::EnablePrivilege(HANDLE hToken, LPCWSTR szPrivName)
{
	TOKEN_PRIVILEGES tkp;
	LookupPrivilegeValue(NULL, szPrivName, &tkp.Privileges[0].Luid);//修改进程权限
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL);//通知系统修改进程权限
	return((GetLastError() == ERROR_SUCCESS));
}
void CTask::SendTask(SOCKET sock)
{
	MSGINFO_S msg;
	TASK task;
	memset(&msg, 0, sizeof(MSGINFO_S));
	msg.Msg_id = TASKLIST;

	DWORD needed;
	HANDLE hProcess;
	HMODULE hModule;
	wchar_t path[260] = _T("");
	HANDLE hToken;

	HANDLE  hProcessSnap = NULL;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		EnablePrivilege(hToken,SE_DEBUG_NAME); //尝试提升权限
		Process32First(hProcessSnap, &pe32);
		do
		{
			memset(&task, 0, sizeof(TASK));
			memset(msg.context, 0, sizeof(msg.context));
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pe32.th32ProcessID);
			if (hProcess)
			{
				EnumProcessModules(hProcess, &hModule, sizeof(hModule), &needed);
				GetModuleFileNameExW(hProcess, hModule, path, sizeof(path));
				wcscpy_s(task.TaskPath, _countof(path), path);
			}
			wcscpy_s(task.TaskName, _countof(pe32.szExeFile), pe32.szExeFile); //进程名
			task.TaskID = pe32.th32ProcessID;
			task.flag = false;
			memcpy(msg.context, &task, sizeof(TASK));
			m_sock.MySend(sock, (char*)&msg, sizeof(MSGINFO_S));
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcess);
	CloseHandle(hModule);

	task.flag = true;
	memcpy(msg.context, &task, sizeof(TASK));
	m_sock.MySend(sock, (char*)&msg, sizeof(MSGINFO_S));
}

void CTask::KillTask(DWORD KillID, SOCKET sock)
{
	MSGINFO_S msg;
	memset(&msg, 0, sizeof(MSGINFO_S));
	msg.Msg_id = TASKKILL;
	HANDLE hProcessHandle;
	hProcessHandle = ::OpenProcess(PROCESS_TERMINATE, FALSE, KillID);
	if (hProcessHandle)
	{
		TerminateProcess(hProcessHandle, 4);
		strcpy_s((char*)msg.context, 2, "T");
	}
	else
	{
		strcpy_s((char*)msg.context, 2, "F");
	}
	m_sock.MySend(sock, (char*)&msg, sizeof(MSGINFO_S));
}
