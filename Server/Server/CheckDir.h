#pragma once


// CCheckDir �Ի���

class CCheckDir : public CDialogEx
{
	DECLARE_DYNAMIC(CCheckDir)

public:
	CCheckDir(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCheckDir();

// �Ի�������
	enum { IDD = IDD_DOWNLOAD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	bool flag;
	//bool iniPath;
	wchar_t path[MAX_PATH];
	afx_msg void OnBnClickedView();
	CString m_strFileOut;
	afx_msg void OnBnClickedOK();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheck1();
	virtual BOOL OnInitDialog();
};
