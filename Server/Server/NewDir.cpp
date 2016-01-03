// NewDir.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "NewDir.h"
#include "afxdialogex.h"
#include "Common.h"


// CNewDir 对话框

IMPLEMENT_DYNAMIC(CNewDir, CDialogEx)

CNewDir::CNewDir(CWnd* pParent /*=NULL*/, SOCKET Socket, BYTE t, wchar_t path[260])
	: CDialogEx(CNewDir::IDD, pParent)
{
	this->type = t;
	m_sock = Socket;
	if (t == 0)
	{
		wcscpy_s(dirpath, path);
	}
}

CNewDir::~CNewDir()
{
}

void CNewDir::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNewDir, CDialogEx)
	ON_BN_CLICKED(IDOK, &CNewDir::OnBnClickedOk)
END_MESSAGE_MAP()


// CNewDir 消息处理程序


void CNewDir::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	MSGINFO msg;
	if (this->type == 0)
	{
		int t;
		t = GetDlgItem(IDC_EDIT_NAME)->GetWindowTextLengthW();
		if (t == 0)
		{
			MessageBox(_T("请输入文件名"), _T("提示"), MB_OK | MB_ICONINFORMATION);
			return;
		}
		else
		{
			msg.Msg_id = NEWDIR;
			wchar_t dirname[180];
			GetDlgItem(IDC_EDIT_NAME)->GetWindowTextW(dirname, _countof(dirname));
			wcscat_s(this->dirpath, dirname);
			memcpy((void*)msg.context, (void*)dirpath, sizeof(dirpath));
		}
	}
	else if (this->type == 1)
	{
		//开启代理端口
		msg.Msg_id = HTTPSOCK;
	}
	else
	{
		msg.Msg_id = URLDOWN;
		//URLDown
	}
	m_MySock.SendCommand(m_sock, (char*)&msg, sizeof(msg));
	SendMessageW(WM_CLOSE);
}

void CNewDir::UpdateUI()
{
	if (type == 1)
	{
		this->SetWindowTextW(_T("开启代理"));
		GetDlgItem(IDC_STATIC)->SetWindowTextW(_T("请输入代理的端口"));
	}
	else if (type == 2)
	{
		this->SetWindowTextW(_T("URL下载"));
		GetDlgItem(IDC_STATIC)->SetWindowTextW(_T("请输入下载地址"));
	}
}

BOOL CNewDir::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (this->type!=0)
	       this->UpdateUI();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
