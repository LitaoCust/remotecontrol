// FileTrans.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "FileTrans.h"
#include "afxdialogex.h"
#include "Common.h"


// CFileTrans �Ի���

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


// CFileTrans ��Ϣ�������
/*
���ܣ�ʵ�����ļ�����Ի����������ʱ�����ݴ���
������1.�ļ����2.Դ�ļ�·����3.����·����4.�ļ���С��5.�б����ʵ�ʴ�С
����ֵ����
*/
void CFileTrans::OnAddFilelist(int type, wchar_t *path1, wchar_t *path2, wchar_t *size, __int64 *usize)
{
	int i = m_list.GetItemCount();
	if (type == 0)
	{
		m_list.InsertItem(i, _T("����"), 0);
	}
	else
	{
		m_list.InsertItem(i, _T("�ϴ�"), 1);
	}
	m_list.SetItemText(i, 1, path1);
	m_list.SetItemText(i, 2, path2);

	////����
	//CString strPath;
	//strPath = m_list.GetItemText(0, 2);
	////����

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
���ܣ��ļ����䣬���ء��ϴ��������list�е�һ��item�������ﴫ����һ������list����ɾ����һ�����ټ��������һ����ֱ��û��item
��������
����ֵ����
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
	if ((m_list.GetItemText(0, 0)).Compare(_T("����")) == 0)
	{
		m_list.GetItemText(0, 2, path, _countof(path));//========================================���⣺pathΪ�� ������������õ�����
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
		//�ϴ�
		MSGINFO msg;
		memset(&msg, 0, sizeof(MSGINFO));
		msg.Msg_id = UPLOAD;
		memset(path, 0, _countof(path));
		m_list.GetItemText(0, 2, path, _countof(path));
		memcpy(msg.context, path, sizeof(path));
		m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(MSGINFO));
		//�����ϴ���Ϣ
		memset(&msg, 0, sizeof(MSGINFO));
		msg.Msg_id = FILEDATA;
		wchar_t upfilepath[260];
		m_list.GetItemText(0, 1, upfilepath, _countof(upfilepath));
		CFile file(upfilepath, CFile::modeRead);
		__int64 u;
		u = file.GetLength();
		DOWNFILEDATA fi;
		fi.flag = true;
		if (u <= 512) //�ļ�����һ����
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
				if (i == Count - 1) //�������һ�δ����ļ�����
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
		//���ͽ�����־
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
���ܣ����տͻ��˴������ļ����ݣ�Ȼ���򱾵ش������ļ�д������
�����������ļ���Ϣ�ṹ�壺DOWNFILEDATA
����ֵ����
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
���ܣ������ļ��Ի����������
��������
����ֵ����
*/
void CFileTrans::UpdateMainFrame()
{
	m_transicon.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 0);
	m_transicon.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DOWN)));
	m_transicon.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_UP)));
	m_list.SetImageList(&m_transicon, LVSIL_SMALL);

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("��������"), LVCFMT_LEFT, 60);
	m_list.InsertColumn(1, _T("Դ�ļ�·��"), LVCFMT_LEFT, 190);
	m_list.InsertColumn(2, _T("Ŀ���ļ�·��"), LVCFMT_LEFT, 190);
	m_list.InsertColumn(3, _T("�ļ���С"), LVCFMT_LEFT, 80);

	m_progress.SetRange(0, 100);
}

BOOL CFileTrans::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	UpdateMainFrame();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
