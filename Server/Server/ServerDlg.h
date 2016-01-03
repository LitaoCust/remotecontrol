
// ServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "ItemData.h"
#include "MySocket.h"
#include "ConfigDlg.h"


// CServerDlg �Ի���
class CServerDlg : public CDialogEx
{
// ����
public:
	CServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SMAIN_DLG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CToolBar m_toolbar;
	CImageList m_imagelist;
	CStatusBar m_statusbar;
	
	CRect m_rect;
	SOCKET s;
	CMySocket m_Mysock;

	
	void UpdateMain(int port);
	bool InitSocket();
	void AddHost(SOCKET sock, SOCKADDR_IN addr);
	void GetNewItemData(SOCKET sock, SOCKADDR_IN addr);
	void  ShowOnLine();
	static DWORD WINAPI OninitSocket(LPVOID self);
	void OnFileManager();
	void OnCmdShell();
	void OnConfig();
	void OnTask();
	void OnBuild();
	bool ReadConfig();
	CItemData*  GetSelItemInfo();
public:
	CListCtrl m_list;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT  OnAddHost(WPARAM wparam, LPARAM lparam);
	LRESULT  OnOffLine(WPARAM wparam, LPARAM lparam);
	afx_msg void OnClose();
	UINT m_port;
	CConfigDlg *m_config;
	UINT m_max;
	UINT m_sound;
	CString m_path;
	wchar_t DownPath[MAX_PATH];
};
