// FileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "FileDlg.h"
#include "afxdialogex.h"
#include "ItemData.h"


// CFileDlg 对话框

IMPLEMENT_DYNAMIC(CFileDlg, CDialogEx)

CFileDlg::CFileDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialogEx(CFileDlg::IDD, pParent)
{
	this->m_sock = sock;
}

CFileDlg::~CFileDlg()
{
}

void CFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}


BEGIN_MESSAGE_MAP(CFileDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CFileDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CFileDlg 消息处理程序


void CFileDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	((CItemData*)this->m_pParentWnd)->m_filedlg = NULL;
	DestroyWindow();
	delete this;

	//CDialogEx::OnClose();
}

void CFileDlg::UpDateMainFrame()
{
	GetClientRect(&m_rect);

	/*m_imagetab.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 0);
	m_imagetab.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON6)));
	m_imagetab.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON5)));*/

	//m_tab.SetImageList(&m_imagetab);
	m_tab.InsertItem(0, _T("文件查看"), 0);
	m_tab.InsertItem(1, _T("文件传输"), 1);

	m_fileview = new CFileView(this, m_sock);
	m_fileview->Create(IDD_FILEVIEW_DLG, &m_tab);

	CRect m_rect;
	m_tab.GetClientRect(m_rect);
	m_rect.DeflateRect(0, 20, 0, 0);
	m_fileview->MoveWindow(m_rect);

	m_filetrans = new CFileTrans(this, m_sock);
	m_filetrans->Create(IDD_FILETRANS_DLG, &m_tab);
	m_tab.GetClientRect(m_rect);
	m_rect.DeflateRect(0, 20, 0, 0);
	m_filetrans->MoveWindow(m_rect);

	m_fileview->ShowWindow(SW_SHOW);

	BOOL hbar = m_statusbar.Create(this);
	UINT b[3] = { 1009, 1010 };
	hbar = m_statusbar.SetIndicators(b, 2);
	m_statusbar.SetPaneInfo(0, b[0], SBPS_NORMAL, 200);
	m_statusbar.SetPaneInfo(1, b[1], SBPS_NORMAL, 200);
	m_statusbar.SetPaneText(0, _T(""));
	m_statusbar.SetPaneText(1, _T(""));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}

void CFileDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	int sel = m_tab.GetCurFocus();
	switch (sel)
	{
	case 0:
	{
			  m_fileview->ShowWindow(SW_SHOW);
			  m_filetrans->ShowWindow(SW_HIDE);
	}
		break;
	case 1:
	{
			  m_fileview->ShowWindow(SW_HIDE);
			  m_filetrans->ShowWindow(SW_SHOW);
	}
		break;
	}
	*pResult = 0;
}


BOOL CFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UpDateMainFrame();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
