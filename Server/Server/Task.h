#pragma once
#include "MySocket.h"
#include "afxcmn.h"
#include "Common.h"

// CTask �Ի���

class CTask : public CDialogEx
{
	DECLARE_DYNAMIC(CTask)

public:
	CTask(CWnd* pParent = NULL,SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CTask();

// �Ի�������
	enum { IDD = IDD_TASK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	void UpDateMainFrame();
	void ShowProcessCount();
	CRect m_rect;
	CMySocket m_Mysock;
	SOCKET m_sock;
	CBitmap m_subbmp[2];
	CStatusBar m_statusbar;

public:
	CListCtrl m_list;
	void GetTask(TASK task);
	//BOOL EnablePrivilege(HANDLE hToken, LPCWSTR szPrivName);
	
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillprc();
	void GetResult(char t[2]);
	afx_msg void OnReflush();
};
