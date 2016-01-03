// ConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"
#include "ServerDlg.h"


// CConfigDlg �Ի���

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CConfigDlg::IDD, pParent)
{
	this->pWnd_s = pParent;
}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CConfigDlg::OnBnClickedView)
	ON_BN_CLICKED(IDC_RADIO_OPEN, &CConfigDlg::OnBnClickedRadioOpen)
	ON_BN_CLICKED(IDC_RADIO_CLOSE, &CConfigDlg::OnBnClickedRadioClose)
	ON_BN_CLICKED(IDC_BUTTON3, &CConfigDlg::OnBnClickedButtonOK)
	ON_BN_CLICKED(IDC_BUTTON4, &CConfigDlg::OnBnClickedCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CConfigDlg ��Ϣ�������


void CConfigDlg::OnBnClickedView()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO   bi;
	wchar_t   Buffer[260];
	wchar_t FullPath[260];
	bi.hwndOwner = m_hWnd;                          //m_hWnd��ĳ���������   
	bi.pidlRoot = NULL;
	bi.pszDisplayName = Buffer;                     //����ѡ���Ŀ¼���Ļ�����   
	bi.lpszTitle = _T("��ѡ��Ŀ¼");                //�����Ĵ��ڵ�������ʾ   
	bi.ulFlags = BIF_EDITBOX;
	bi.lpfn = NULL;                                 //�ص���������ʱ����   
	bi.lParam = 0;
	bi.iImage = 0;
	ITEMIDLIST *pidl = ::SHBrowseForFolder(&bi);    //��ʾ�������ڣ�ITEMIDLIST����Ҫ   
	::SHGetPathFromIDList(pidl, FullPath);
	if (pidl == NULL)
	{
		return;
	}
	else
	{
		SHGetPathFromIDList(pidl, FullPath);
		GetDlgItem(IDC_EDIT_PATH)->SetWindowTextW(FullPath);
	}
}


void CConfigDlg::OnBnClickedRadioOpen()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	((CButton *)GetDlgItem(IDC_RADIO_OPEN))->SetCheck(TRUE);//ѡ��
	((CButton *)GetDlgItem(IDC_RADIO_CLOSE))->SetCheck(FALSE);//��ѡ��
}


void CConfigDlg::OnBnClickedRadioClose()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	((CButton *)GetDlgItem(IDC_RADIO_OPEN))->SetCheck(FALSE);//ѡ��
	((CButton *)GetDlgItem(IDC_RADIO_CLOSE))->SetCheck(TRUE);//��ѡ��
}


void CConfigDlg::OnBnClickedButtonOK()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL flag;
	UINT ui = GetDlgItemInt(IDC_EDIT_PORT, &flag, false);
	UINT ui2 = GetDlgItemInt(IDC_EDIT_LINKS, &flag, false);

	int tempPort;
	int tempMax;
	CString tempPath;
	UINT isSound;

	//port
	if (flag)
	{
		((CServerDlg*)this->pWnd_s)->m_port = ui;
		tempPort = ui;
		

	}
	else
	{
		MessageBox(_T("�˿����ô���"), _T("����"), MB_OK | MB_ICONWARNING);
		return;
	}

	//open sound?
	flag = ((CButton *)GetDlgItem(IDC_RADIO_OPEN))->GetCheck() == 1 ? 1 : 0;
	((CServerDlg*)this->pWnd_s)->m_sound = (UINT)flag;
	isSound = (UINT)flag;

	//max linkers
	ui = GetDlgItemInt(IDC_EDIT_LINKS, &flag, false);
	if (flag)
	{
		((CServerDlg*)this->pWnd_s)->m_max = ui;
		tempMax = ui;
	}
	else
	{
		MessageBox(_T("�����������ô���"), _T("����"), MB_OK | MB_ICONWARNING);
		return;
	}


	this->GetDlgItemTextW(IDC_EDIT_PATH, tempPath);
	((CServerDlg*)this->pWnd_s)->m_path = tempPath;
	WriteConfig(tempPort,tempMax,tempPath,isSound);
	CConfigDlg::OnOK();
}


void CConfigDlg::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CConfigDlg::OnClose();

}

bool CConfigDlg::WriteConfig(int port,int max,CString path,int sound)
{
	CString Port;
	CString Max;
	CString Sound;
	Port.Format(L"%d", port);
	Max.Format(L"%d", max);
	Sound.Format(L"%d", sound);

	

	CString  strPath;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos = strPath.ReverseFind('\\');
	strPath = strPath.Left(nPos);
	strPath += "\\config.ini";

	/*CString path;
	this->GetDlgItemTextW(IDC_EDIT_PATH,path);*/
	WritePrivateProfileString(L"Config", L"Port",Port, strPath);
	WritePrivateProfileString(L"Config", L"Sound",Sound,strPath);
	WritePrivateProfileString(L"Config", L"Max", Max, strPath);
	WritePrivateProfileString(L"Config", L"Path", path, strPath);

	return true;
}
bool  CConfigDlg::ReadConfig()
{
	CString port;
	CString max;
	CString sound;
	CString path;
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
	GetPrivateProfileString(L"Config", L"Path", L"C:\\Users\\Public\\Downloads", path.GetBuffer(MAX_PATH), MAX_PATH, strPath);

	this->GetDlgItem(IDC_EDIT_PORT)->SetWindowTextW(port);
	this->GetDlgItem(IDC_EDIT_LINKS)->SetWindowTextW(max);
	this->GetDlgItem(IDC_EDIT_PATH)->SetWindowTextW(path);
	
	if (sound == L"1")
		((CButton *)GetDlgItem(IDC_RADIO_OPEN))->SetCheck(TRUE);//ѡ��
	else
		((CButton *)GetDlgItem(IDC_RADIO_CLOSE))->SetCheck(TRUE);//��ѡ��

	return true;



}

void CConfigDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	((CServerDlg*)this->pWnd_s)->m_config = NULL;
	DestroyWindow();
	delete this;
	//CDialogEx::OnClose();
}


BOOL CConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	this->ReadConfig();
	//((CButton *)GetDlgItem(IDC_RADIO_OPEN))->SetCheck(TRUE);//ѡ��
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
