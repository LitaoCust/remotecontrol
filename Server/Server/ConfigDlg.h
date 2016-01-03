#pragma once


// CConfigDlg �Ի���

class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigDlg();

// �Ի�������
	enum { IDD = IDD_CONFIG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	LPVOID pWnd_s;
	
public:
	afx_msg void OnBnClickedView();
	afx_msg void OnBnClickedRadioOpen();
	afx_msg void OnBnClickedRadioClose();
	afx_msg void OnBnClickedButtonOK();
	afx_msg void OnBnClickedCancel();
	bool WriteConfig(int port, int max, CString path, int sound);
	bool ReadConfig();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
};
