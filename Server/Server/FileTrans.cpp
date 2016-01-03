// FileTrans.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "FileTrans.h"
#include "afxdialogex.h"
#include "Common.h"


// CFileTrans 对话框

IMPLEMENT_DYNAMIC(CFileTrans, CDialogEx)

CFileTrans::CFileTrans(CWnd* pParent /*=NULL*/,SOCKET sock)
	: CDialogEx(CFileTrans::IDD, pParent)
{
	this->m_sock = sock;
	BeginToWork = false;
}

CFileTrans::~CFileTrans()
{
}

void CFileTrans::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CFileTrans, CDialogEx)
END_MESSAGE_MAP()


// CFileTrans 消息处理程序
/*
功能：实现向文件传输对话框添加数据时的数据处理
参数：1.文件类别，2.源文件路径，3.下载路径，4.文件大小，5.列表项的实际大小
返回值：无
*/
void CFileTrans::OnAddFilelist(int type, wchar_t *path1, wchar_t *path2, wchar_t *size, __int64 *usize)
{
	int i = m_list.GetItemCount();
	if (type == 0)
	{
		m_list.InsertItem(i, _T("下载"), 0);
	}
	else
	{
		m_list.InsertItem(i, _T("上传"), 1);
	}
	m_list.SetItemText(i, 1, path1);
	m_list.SetItemText(i, 2, path2);

	////测试
	//CString strPath;
	//strPath = m_list.GetItemText(0, 2);
	////测试

	m_list.SetItemText(i, 3, size);
	m_list.SetItemData(i, (DWORD)usize);
	if (BeginToWork == false)
	{
		BeginToWork = true;
		::CloseHandle(CreateThread(0, 0, BeginToTrans, (LPVOID)this, 0, 0));
	}
}

DWORD WINAPI CFileTrans::BeginToTrans(LPVOID self)
{
	CFileTrans* t = (CFileTrans*)self;
	t->OnFirstTrans();
	return 0;
}

/*
功能：文件传输，下载、上传。传输的list中第一个item，函数里传送完一个，从list里面删除第一个，再继续传输第一个，直到没有item
参数：无
返回值：无
*/
void CFileTrans::OnFirstTrans()
{
	m_progress.SetPos(0);
	if (m_list.GetItemCount() == 0)
	{
		BeginToWork = false;
		return;
	}
	wchar_t path[260];
	//CString strPath;
	if ((m_list.GetItemText(0, 0)).Compare(_T("下载")) == 0)
	{
		m_list.GetItemText(0, 2, path, _countof(path));//========================================问题：path为空 解决：界面设置的问题
		//strPath = m_list.GetItemText(0, 2);
		m_file = new CFile(path, CFile::modeCreate | CFile::modeReadWrite);
		Count = 0;
		MSGINFO msg;
		memset(&msg, 0, sizeof(MSGINFO));
		msg.Msg_id = DOWNLOAD;
		memset(path, 0, _countof(path));
		m_list.GetItemText(0, 1, path, _countof(path));
		memcpy(msg.context, path, sizeof(path));
		m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(MSGINFO));
	}
	else
	{
		//上传
		MSGINFO msg;
		memset(&msg, 0, sizeof(MSGINFO));
		msg.Msg_id = UPLOAD;
		memset(path, 0, _countof(path));
		m_list.GetItemText(0, 2, path, _countof(path));
		memcpy(msg.context, path, sizeof(path));
		m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(MSGINFO));
		//发送上传信息
		memset(&msg, 0, sizeof(MSGINFO));
		msg.Msg_id = FILEDATA;
		wchar_t upfilepath[260];
		m_list.GetItemText(0, 1, upfilepath, _countof(upfilepath));
		CFile file(upfilepath, CFile::modeRead);
		__int64 u;
		u = file.GetLength();
		DOWNFILEDATA fi;
		fi.flag = true;
		if (u <= 512) //文件不足一个块
		{
			file.Read(fi.context, (UINT)u);
			fi.size = (UINT)u;
			fi.count = 0;
			memcpy(msg.context, &fi, sizeof(DOWNFILEDATA));
			m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(MSGINFO));
			m_progress.SetPos(100);
		}
		else
		{
			ULONGLONG Count;
			Count = u / 512 + 1;
			int Last = u % 512;
			double p;
			for (int i = 0; i<Count; i++)
			{
				memset(fi.context, 0, 512);
				if (i == Count - 1) //这是最后一次传送文件数据
				{
					file.Read(fi.context, Last);
					fi.size = Last;
				}
				else
				{
					file.Read(fi.context, 512);
					fi.size = 512;
				}
				fi.count = i;
				memcpy(msg.context, &fi, sizeof(DOWNFILEDATA));
				m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(MSGINFO));
				p = (i + 1) * 1.0 / Count;
				m_progress.SetPos(int(p * 100));
			}
		}
		//发送结束标志
		fi.flag = false;
		memcpy(msg.context, &fi, sizeof(DOWNFILEDATA));
		m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(MSGINFO));
		file.Close();
		::CloseHandle(file.m_hFile);
		m_list.DeleteItem(0);
		::CloseHandle(CreateThread(0, 0, BeginToTrans, (LPVOID)this, 0, 0));
	}
}

/*
功能：接收客户端传来的文件数据，然后向本地创建的文件写入数据
参数：下载文件信息结构体：DOWNFILEDATA
返回值：无
*/
void CFileTrans::GetFileData(DOWNFILEDATA fi)
{
	if (fi.flag == true)
	{
		Count++;
		float pos;
		m_file->Write(fi.context, fi.size);
		pos = float(m_file->GetLength() * 1.0 / *(__int64*)m_list.GetItemData(0));
		m_progress.SetPos(int(pos * 100));
	}
	else
	{
		m_file->Close();
		::CloseHandle(m_file->m_hFile);
		delete m_file;
		m_list.DeleteItem(0);
		::CloseHandle(CreateThread(0, 0, BeginToTrans, (LPVOID)this, 0, 0));
	}
}

/*
功能：传输文件对话框界面设置
参数：无
返回值：无
*/
void CFileTrans::UpdateMainFrame()
{
	m_transicon.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 0);
	m_transicon.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DOWN)));
	m_transicon.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_UP)));
	m_list.SetImageList(&m_transicon, LVSIL_SMALL);

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("传输类型"), LVCFMT_LEFT, 60);
	m_list.InsertColumn(1, _T("源文件路径"), LVCFMT_LEFT, 190);
	m_list.InsertColumn(2, _T("目标文件路径"), LVCFMT_LEFT, 190);
	m_list.InsertColumn(3, _T("文件大小"), LVCFMT_LEFT, 80);

	m_progress.SetRange(0, 100);
}

BOOL CFileTrans::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UpdateMainFrame();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
