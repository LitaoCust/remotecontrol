// Task.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "Task.h"
#include "afxdialogex.h"


// CTask 对话框

IMPLEMENT_DYNAMIC(CTask, CDialogEx)

CTask::CTask(CWnd* pParent /*=NULL*/,SOCKET sock)
	: CDialogEx(CTask::IDD, pParent)
{
	m_sock = sock;
}

CTask::~CTask()
{
}

void CTask::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CTask, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CTask::OnNMRClickList1)
	ON_COMMAND(ID_KILLPRC, &CTask::OnKillprc)
	ON_COMMAND(ID_REFLUSH, &CTask::OnReflush)
END_MESSAGE_MAP()


// CTask 消息处理程序


BOOL CTask::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetClientRect(&m_rect);
	UpDateMainFrame();
	MSGINFO msg;
	memset(&msg, 0, sizeof(MSG));
	msg.Msg_id = TASKLIST;
	if (m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg)) == SOCKET_ERROR)
	{
		MessageBox(_T("启用获取进程命令发送失败"), _T("错误"), MB_OK | MB_ICONWARNING);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CTask::UpDateMainFrame()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("进程名称"), LVCFMT_LEFT, 130);
	m_list.InsertColumn(1, _T("进程标识"), LVCFMT_LEFT, 80);
	m_list.InsertColumn(2, _T("进程路径"), LVCFMT_LEFT, 300);

	//m_subbmp[0].LoadBitmapW(IDB_MENU_TASKKILL);
	//m_subbmp[1].LoadBitmapW(IDB_MENU_REFRESH);

	BOOL hbar = m_statusbar.Create(this);
	UINT b = 1001;
	hbar = m_statusbar.SetIndicators(&b, 1);
	m_statusbar.SetPaneInfo(0, b, SBPS_NORMAL, 400);
	m_statusbar.SetPaneText(0, _T("进程数：0"));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}

void CTask::GetTask(TASK task)
{
	if (task.flag == true)
	{
		return;
	}
	CString csID;
	csID.Format(_T("%d"), task.TaskID);
	int i = m_list.GetItemCount();
	m_list.InsertItem(i, task.TaskName);
	m_list.SetItemText(i, 1, csID);
	m_list.SetItemText(i, 2, task.TaskPath);
	m_list.SetItemData(i, task.TaskID);
	ShowProcessCount();
}

void CTask::ShowProcessCount()
{
	int Count = m_list.GetItemCount();
	TCHAR* szText;
	CString TaskCount;
	TaskCount.Format(_T("进程数：%d"), Count);
	szText = TaskCount.GetBuffer(TaskCount.GetLength());
	::SendMessageW(m_statusbar, SB_SETTEXT, (WPARAM)0, (LPARAM)szText);
}



void CTask::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	CMenu menu, *pSubMenu;//定义下面要用到的cmenu对象 
	menu.LoadMenu(IDR_MENU3);//装载自定义的右键菜单 
	pSubMenu = menu.GetSubMenu(0);
	//pSubMenu->SetMenuItemBitmaps(0, MF_BYPOSITION, &m_subbmp[0], &m_subbmp[0]);
	//pSubMenu->SetMenuItemBitmaps(1, MF_BYPOSITION, &m_subbmp[1], &m_subbmp[1]);
	if (pNMItemActivate->iItem == -1)
	{
		pSubMenu->EnableMenuItem(ID_KILLPRC, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	//获取第一个弹出菜单，所以第一个菜单必须有子菜单 
	CPoint oPoint;//定义一个用于确定光标位置的位置 
	GetCursorPos(&oPoint);//获取当前光标的位置，以便使得菜单可以跟随光标 
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this);
	*pResult = 0;
}


void CTask::OnKillprc()
{
	// TODO:  在此添加命令处理程序代码
	MSGINFO msg;
	memset(&msg, 0, sizeof(MSGINFO));
	msg.Msg_id = TASKKILL;
	TASK task;
	memset(&task, 0, sizeof(TASK));
	POSITION pos_s;
	pos_s = m_list.GetFirstSelectedItemPosition();
	if (!pos_s)
	{
		return;
	}
	int item = m_list.GetNextSelectedItem(pos_s);
	task.TaskID = m_list.GetItemData(item);
	memcpy(msg.context, (char*)&task, sizeof(TASK));
	m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(MSGINFO));
}

void CTask::GetResult(char t[2])
{
	if (strcmp(t, "T") == 0)
	{
		MessageBox(_T("结束进程成功"), _T("提示"), MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(_T("结束进程失败"), _T("提示"), MB_OK | MB_ICONWARNING);
	}
	OnReflush();
}

void CTask::OnReflush()
{
	// TODO:  在此添加命令处理程序代码
	m_list.DeleteAllItems();
	MSGINFO msg;
	memset(&msg, 0, sizeof(MSG));
	msg.Msg_id = TASKLIST;
	if (m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg)) == SOCKET_ERROR)
	{
		MessageBox(_T("启用获取进程命令发送失败"), _T("错误"), MB_OK | MB_ICONWARNING);
	}
}


