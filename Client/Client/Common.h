#pragma once

#include <windows.h>
#define SYSINFO  0x01
#define DISKINFO 0x02
#define FILELIST 0x03
#define DOWNLOAD 0x04
#define UPLOAD   0x05 
#define FILEDATA 0x06
#define RUNEXE   0x07
#define DELFILE  0x09
#define NEWDIR   0x0a
#define COMMAND  0x0d
#define CMDSHELL 0x0e
#define TASKLIST 0x0f
#define TASKKILL 0x10



typedef struct tagMSGINFO //传输消息结构体
{
	int Msg_id;
	BYTE context[1024 * 5];
}MSGINFO_S;

typedef struct tagSYSTEMINFO
{
	int os;
	bool Cam; //摄像头
	double ver;
}SYSTEMINFO_S;

typedef struct tagDRIVER
{
	wchar_t disk;
	double all;
	double free;
	int type;
}DRIVER;

typedef struct tagFILEINFO
{
	TCHAR FileName[256];
	int type;
	__int64 size;
}FILEINFO;

typedef struct tagDOWNFILEDATA
{
	bool flag;
	__int64 size;
	__int64 count;
	BYTE context[512];
}DOWNFILEDATA;

typedef struct tagCMD //CMD命令信息
{
	int flag;
	char command[1024];
}CMD;

typedef struct tagTASK
{
	wchar_t TaskName[260];
	DWORD   TaskID;
	wchar_t TaskPath[260];
	bool flag;
}TASK;