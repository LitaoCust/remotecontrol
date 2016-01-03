#pragma once
#include "afxcmn.h"
#include "Resource.h"
#include "FileView.h"
#include "FileTrans.h"


// CFileDlg 对话框

class CFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileDlg)

public:
	CFileDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CFileDlg();

// 对话框数据
	enum { IDD = IDD_FILE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tab;
private:
	SOCKET m_sock;
	CRect m_rect;
	
	
	void UpDateMainFrame();
public:
	afx_msg void OnClose();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	CFileView* m_fileview;
	CFileTrans* m_filetrans;
	CStatusBar m_statusbar;
};
