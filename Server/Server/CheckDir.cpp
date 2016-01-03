// CheckDir.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "CheckDir.h"
#include "afxdialogex.h"


// CCheckDir �Ի���

IMPLEMENT_DYNAMIC(CCheckDir, CDialogEx)

CCheckDir::CCheckDir(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCheckDir::IDD, pParent)
{

}

CCheckDir::~CCheckDir()
{
}

void CCheckDir::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCheckDir, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CCheckDir::OnBnClickedView)
	ON_BN_CLICKED(IDC_BUTTON2, &CCheckDir::OnBnClickedOK)
	ON_BN_CLICKED(IDC_BUTTON3, &CCheckDir::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK1, &CCheckDir::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CCheckDir ��Ϣ�������


void CCheckDir::OnBnClickedView()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//wchar_t szSelected[MAX_PATH]; //��������ļ���·��
	//BROWSEINFO bi;
	//LPITEMIDLIST pidl;
	//bi.hwndOwner = this->m_hWnd;
	//bi.pidlRoot = NULL;
	//bi.pszDisplayName = szSelected;
	//bi.lpszTitle = L"ѡ���ļ�·��";
	//bi.ulFlags = BIF_RETURNONLYFSDIRS;
	//bi.lpfn = NULL;
	//bi.lParam = NULL;
	//bi.iImage = NULL;
	//if ((pidl = SHBrowseForFolder(&bi)) != NULL)
	//{
	//	if (SUCCEEDED(SHGetPathFromIDList(pidl, szSelected))) //�õ��ļ��е�ȫ·������Ҫ�Ļ���ֻ�ñ��ļ�����
	//	{
	//		m_strFileOut = szSelected;
	//	}
	//}
	//SetDlgItemText(IDC_EDIT1, m_strFileOut); //��ʾ·����
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
		GetDlgItem(IDC_EDIT1)->SetWindowTextW(FullPath);
	}
}
	




void CCheckDir::OnBnClickedOK()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	GetDlgItem(IDC_EDIT1)->GetWindowTextW(path, _countof(path));//=========================������!
	
	if (!PathFileExistsW(path))
	{
		MessageBox(_T("����·�������ڣ�"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		return;
	}
	this->flag = true;
	if (IsDlgButtonChecked(IDC_CHECK1) == BST_CHECKED)
	{
		CString  strPath;
		GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
		strPath.ReleaseBuffer();
		int nPos = strPath.ReverseFind('\\');
		strPath = strPath.Left(nPos);
		strPath += "\\config.ini";

		CString path;
		this->GetDlgItemTextW(IDC_EDIT1, path);
		WritePrivateProfileString(L"Config", L"Path", path, strPath);
	}

	this->EndDialog(IDOK);
	
}


void CCheckDir::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	this->flag = false;
	this->EndDialog(IDCANCEL);
}


void CCheckDir::OnBnClickedCheck1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//this->iniPath = !this->iniPath;
	
	

}


BOOL CCheckDir::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	this->flag = false;
	//this->iniPath = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
