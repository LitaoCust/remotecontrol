// NewDir.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "NewDir.h"
#include "afxdialogex.h"
#include "Common.h"


// CNewDir �Ի���

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


// CNewDir ��Ϣ�������


void CNewDir::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	MSGINFO msg;
	if (this->type == 0)
	{
		int t;
		t = GetDlgItem(IDC_EDIT_NAME)->GetWindowTextLengthW();
		if (t == 0)
		{
			MessageBox(_T("�������ļ���"), _T("��ʾ"), MB_OK | MB_ICONINFORMATION);
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
		//��������˿�
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
		this->SetWindowTextW(_T("��������"));
		GetDlgItem(IDC_STATIC)->SetWindowTextW(_T("���������Ķ˿�"));
	}
	else if (type == 2)
	{
		this->SetWindowTextW(_T("URL����"));
		GetDlgItem(IDC_STATIC)->SetWindowTextW(_T("���������ص�ַ"));
	}
}

BOOL CNewDir::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	if (this->type!=0)
	       this->UpdateUI();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
