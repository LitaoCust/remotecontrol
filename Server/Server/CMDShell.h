#pragma once
#include "afxwin.h"
#include "MySocket.h"
#include "StringToTransform.h"
#include "Common.h"
// CCMDShell �Ի���

class CCMDShell : public CDialogEx
{
	DECLARE_DYNAMIC(CCMDShell)

public:
	CCMDShell(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CCMDShell();

// �Ի�������
	enum { IDD = IDD_CMDSHELL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
