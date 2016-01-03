#pragma once
#include "MySocket.h"

// CNewDir 对话框

class CNewDir : public CDialogEx
{
	DECLARE_DYNAMIC(CNewDir)

public:
	CNewDir(CWnd* pParent /*=NULL*/, SOCKET Socket, BYTE t, wchar_t path[260]);   // 标准构造函数
	virtual ~CNewDir();

// 对话框数据
	enum { IDD = IDD_NEWDIR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	int type;
	SOCKET m_sock;
	CMySocket m_MySock;
	wchar_t dirpath[MAX_PATH];
public:
	afx_msg void OnBnClickedOk();
	void UpdateUI();
	virtual BOOL OnInitDialog();
};
