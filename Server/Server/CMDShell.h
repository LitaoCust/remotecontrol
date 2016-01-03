#pragma once
#include "afxwin.h"
#include "MySocket.h"
#include "StringToTransform.h"
#include "Common.h"
// CCMDShell 对话框

class CCMDShell : public CDialogEx
{
	DECLARE_DYNAMIC(CCMDShell)

public:
	CCMDShell(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CCMDShell();

// 对话框数据
	enum { IDD = IDD_CMDSHELL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	SOCKET m_sock;
	CMySocket m_Mysock;
	CStringToTransform m_str;
	CRect m_rect;
	//MSGINFO msg;
public:
	CEdit m_recv;
	CEdit m_send;
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	void GetReturnInfo(CMD t);
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	//static unsigned int __stdcall OnCmdRecv(LPVOID self);
	//void OnCmdRecv_run();
	virtual void PostNcDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
