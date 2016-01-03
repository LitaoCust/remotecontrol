// CMDShell.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "CMDShell.h"
#include "afxdialogex.h"
#include "Common.h"
#include "ItemData.h"


// CCMDShell �Ի���

IMPLEMENT_DYNAMIC(CCMDShell, CDialogEx)

CCMDShell::CCMDShell(CWnd* pParent /*= NULL*/, SOCKET sock)
	: CDialogEx(CCMDShell::IDD, pParent)
{
	this->m_sock = sock;
}

CCMDShell::~CCMDShell()
{
}

void CCMDShell::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMDRecv, m_recv);
	DDX_Control(pDX, IDC_CMDSend, m_send);
}


BEGIN_MESSAGE_MAP(CCMDShell, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCMDShell ��Ϣ�������


BOOL CCMDShell::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	MSGINFO msg;
	memset(&msg, 0, sizeof(MSGINFO));
	msg.Msg_id = CMDSHELL;
	if (m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg)) == SOCKET_ERROR)
	{
		MessageBox(_T("����CMD�����ʧ��"), _T("����"), MB_OK | MB_ICONINFORMATION);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CCMDShell::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	MSGINFO msg;
	CMD cmd;
	memset(&msg, 0, sizeof(MSGINFO));
	memset(&cmd, 0, sizeof(CMD));
	msg.Msg_id = COMMAND;
	strcpy_s((char*)cmd.command, sizeof("exit\r\n"), "exit\r\n");
	memcpy(msg.context, &cmd, sizeof(CMD));
	m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg));

	((CItemData*)this->m_pParentWnd)->m_cmdshell = NULL;
	DestroyWindow();
	delete this;//<-------------------------bug�����
	//this->m_hWnd

}
void CCMDShell::GetReturnInfo(CMD t)
{
	if (t.flag == 0)
	{
		CString tem;
		wchar_t *pwText;
		pwText = m_str.CharToCString(t.command);
		m_recv.GetWindowTextW(tem);
		m_recv.SetWindowTextW(tem + pwText);
		m_recv.SetSel(-1);
		delete[] pwText;
		m_send.GetFocus();
	}
	else
	{
		MessageBox(_T("�����CMD���̱��������"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		SendMessageW(WM_CLOSE, 0, 0);
	}
}


BOOL CCMDShell::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (nVirtKey == VK_RETURN)
		{
			//������Ϣ
			if (m_send.GetSafeHwnd() == ::GetFocus())
			{
				if (GetDlgItem(IDC_CMDSend)->GetWindowTextLengthW() == 0)
				{
					return TRUE;
				}
				CString str;
				GetDlgItem(IDC_CMDSend)->GetWindowTextW(str);
				MSGINFO msg;                              //һ��Ҫ��������������
				msg.Msg_id = COMMAND;
				CMD cmd;
				memset(&cmd, 0, sizeof(CMD));
				memset(&msg, 0, sizeof(MSGINFO));
				m_str.CStringToChar(str, cmd.command);
				if (strcmp(cmd.command, "exit") == 0)
				{
					SendMessageW(WM_CLOSE, 0, 0);
					return TRUE;
				}
				msg.Msg_id = COMMAND;
				strcat_s((char*)cmd.command, sizeof(cmd.command), "\r\n");
				memcpy(msg.context, &cmd, sizeof(CMD));
				m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg));
				GetDlgItem(IDC_CMDSend)->SetWindowTextW(_T(""));
				str.ReleaseBuffer();
			}
			return TRUE;
		}
		else if (nVirtKey == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CCMDShell::PostNcDestroy()
{
	// TODO:  �ڴ����ר�ô����/����û���

	CDialogEx::PostNcDestroy();
	//delete this;
}


void CCMDShell::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	if (nType == SIZE_MINIMIZED)
	{
		return;
	}
	CWnd *pWnd, *pWnd1;
	pWnd = GetDlgItem(IDC_CMDRecv);     //��ȡ�ؼ����
	pWnd1 = GetDlgItem(IDC_CMDSend);
	if (pWnd && pWnd1)//�ж��Ƿ�Ϊ�գ���Ϊ�Ի��򴴽�ʱ����ô˺���������ʱ�ؼ���δ����
	{
		CRect rect, rect_l;   //��ȡ�ؼ��仯ǰ��С
		GetClientRect(&rect_l);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right = rect.right + (rect_l.right - m_rect.right);
		rect.bottom = rect.bottom + (rect_l.bottom - m_rect.bottom);
		pWnd->MoveWindow(rect);//���ÿؼ���С

		pWnd1->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.top = rect.top + (rect_l.bottom - m_rect.bottom);
		rect.right = rect.right + (rect_l.right - m_rect.right);
		rect.bottom = rect.bottom + (rect_l.bottom - m_rect.bottom);
		pWnd1->MoveWindow(rect);//���ÿؼ���С
	}
	else
	{
		delete pWnd;
	}
	GetClientRect(&m_rect);
}
