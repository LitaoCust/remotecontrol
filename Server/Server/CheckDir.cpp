// CheckDir.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "CheckDir.h"
#include "afxdialogex.h"


// CCheckDir 对话框

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


// CCheckDir 消息处理程序


void CCheckDir::OnBnClickedView()
{
	// TODO:  在此添加控件通知处理程序代码
	//wchar_t szSelected[MAX_PATH]; //用来存放文件夹路径
	//BROWSEINFO bi;
	//LPITEMIDLIST pidl;
	//bi.hwndOwner = this->m_hWnd;
	//bi.pidlRoot = NULL;
	//bi.pszDisplayName = szSelected;
	//bi.lpszTitle = L"选择文件路径";
	//bi.ulFlags = BIF_RETURNONLYFSDIRS;
	//bi.lpfn = NULL;
	//bi.lParam = NULL;
	//bi.iImage = NULL;
	//if ((pidl = SHBrowseForFolder(&bi)) != NULL)
	//{
	//	if (SUCCEEDED(SHGetPathFromIDList(pidl, szSelected))) //得到文件夹的全路径，不要的话，只得本文件夹名
	//	{
	//		m_strFileOut = szSelected;
	//	}
	//}
	//SetDlgItemText(IDC_EDIT1, m_strFileOut); //显示路径名
	BROWSEINFO   bi;
	wchar_t   Buffer[260];
	wchar_t FullPath[260];
	bi.hwndOwner = m_hWnd;                          //m_hWnd你的程序主窗口   
	bi.pidlRoot = NULL;
	bi.pszDisplayName = Buffer;                     //返回选择的目录名的缓冲区   
	bi.lpszTitle = _T("请选择目录");                //弹出的窗口的文字提示   
	bi.ulFlags = BIF_EDITBOX;
	bi.lpfn = NULL;                                 //回调函数，有时很用   
	bi.lParam = 0;
	bi.iImage = 0;
	ITEMIDLIST *pidl = ::SHBrowseForFolder(&bi);    //显示弹出窗口，ITEMIDLIST很重要   
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
	// TODO:  在此添加控件通知处理程序代码

	GetDlgItem(IDC_EDIT1)->GetWindowTextW(path, _countof(path));//=========================在这里!
	
	if (!PathFileExistsW(path))
	{
		MessageBox(_T("下载路径不存在！"), _T("提示"), MB_OK | MB_ICONWARNING);
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
	// TODO:  在此添加控件通知处理程序代码
	this->flag = false;
	this->EndDialog(IDCANCEL);
}


void CCheckDir::OnBnClickedCheck1()
{
	// TODO:  在此添加控件通知处理程序代码
	//this->iniPath = !this->iniPath;
	
	

}


BOOL CCheckDir::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	this->flag = false;
	//this->iniPath = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
