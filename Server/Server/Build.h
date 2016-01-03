#pragma once


// CBuild �Ի���

class CBuild : public CDialogEx
{
	DECLARE_DYNAMIC(CBuild)

public:
	CBuild(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBuild();

// �Ի�������
	enum { IDD = IDD_BUILD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	void FileToMem(wchar_t* pOldFilePathName);
	void MemToFile(wchar_t* pNewFilePathName);
	void UpdateString(char *pOldStr, char *pNewStr, int stroldlen, int strnewlen);
	bool CheckInput();
	void WriteInfoToFile();
	void CStringToChar(wchar_t *wIP, char *Info);
	void CStringToChar(CString str, char* w);
	CFile m_File;
	ULONGLONG d_FileSize;
	BYTE* pFileData;

	
public:
	afx_msg void OnBnClickedOk();
	unsigned int m_port;
};
