#pragma once
#include "MySocket.h"

// CNewDir �Ի���

class CNewDir : public CDialogEx
{
	DECLARE_DYNAMIC(CNewDir)

public:
	CNewDir(CWnd* pParent /*=NULL*/, SOCKET Socket, BYTE t, wchar_t path[260]);   // ��׼���캯��
	virtual ~CNewDir();

// �Ի�������
	enum { IDD = IDD_NEWDIR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
