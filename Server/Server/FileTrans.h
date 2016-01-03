#pragma once
#include "MySocket.h"
#include "afxcmn.h"
#include "Common.h"


// CFileTrans 对话框

class CFileTrans : public CDialogEx
{
	DECLARE_DYNAMIC(CFileTrans)

public:
	CFileTrans(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CFileTrans();

// 对话框数据
	enum { IDD = IDD_FILETRANS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_sock;
	bool BeginToWork;
	
	//CProgressCtrl m_progress;
	CFile *m_file;
	int Count;
	CMySocket m_Mysock;
	CImageList m_transicon;
	
	static DWORD WINAPI BeginToTrans(LPVOID self);
	
	
	void UpdateMainFrame();
public:
	CProgressCtrl m_progress;
	void OnAddFilelist(int type, wchar_t *path1, wchar_t *path2, wchar_t *size, __int64 *usize);
	CListCtrl m_list;
	virtual BOOL OnInitDialog();
	void OnFirstTrans();
	void GetFileData(DOWNFILEDATA fi);
};
