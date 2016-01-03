#pragma once
#include "MySocket.h"
#include "afxcmn.h"
#include "Common.h"
#include "StringToTransform.h"


// CFileView 对话框

class CFileView : public CDialogEx
{
	DECLARE_DYNAMIC(CFileView)

public:
	CFileView(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CFileView();

// 对话框数据
	enum { IDD = IDD_FILEVIEW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

DECLARE_MESSAGE_MAP()
private:
	LPVOID pWnd_s;       //父窗口指针
	SOCKET m_sock;       //用于通信的socket，是客户端对象送来的
	CMySocket m_Mysock;    //用于调用CMysocket中的方法
	CRect m_rect;
	CImageList m_imagetree;
	CImageList m_imagelist;
	int DirCount;      //文件夹的数量？
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
	void SetShowInfo(HTREEITEM Item); //显示磁盘及文件信息
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
