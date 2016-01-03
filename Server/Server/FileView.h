#pragma once
#include "MySocket.h"
#include "afxcmn.h"
#include "Common.h"
#include "StringToTransform.h"


// CFileView �Ի���

class CFileView : public CDialogEx
{
	DECLARE_DYNAMIC(CFileView)

public:
	CFileView(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CFileView();

// �Ի�������
	enum { IDD = IDD_FILEVIEW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

DECLARE_MESSAGE_MAP()
private:
	LPVOID pWnd_s;       //������ָ��
	SOCKET m_sock;       //����ͨ�ŵ�socket���ǿͻ��˶���������
	CMySocket m_Mysock;    //���ڵ���CMysocket�еķ���
	CRect m_rect;
	CImageList m_imagetree;
	CImageList m_imagelist;
	int DirCount;      //�ļ��е�������
	bool flag;
	HTREEITEM hItem;

	CStringToTransform forTransform;

public:
	void UpdateMainFrame();
	CTreeCtrl m_tree;
	CListCtrl m_list;
	virtual BOOL OnInitDialog();
	void setDisk(DRIVER disk);
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	CString GetPath(HTREEITEM item);
	void DelChilditem(HTREEITEM hItem);
	void FileList(FILEINFO fileinfo);
	CString GetRealSize(__int64 size);
	void SetShowInfo(HTREEITEM Item); //��ʾ���̼��ļ���Ϣ
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFilecontrolDown();
	afx_msg void OnFileUpLoad();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	HTREEITEM GetSelectTreeItem(CString t);
	afx_msg void OnBnClickedButtonBack();
	afx_msg void OnFilecontrolRunfile();
	afx_msg void OnFilecontrolDel();
	afx_msg void OnFileNewDir();
	afx_msg void OnFileRefresh();
};
