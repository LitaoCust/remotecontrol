// FileView.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "FileView.h"
#include "afxdialogex.h"
#include "FileDlg.h"
#include "CheckDir.h"
#include "NewDir.h"



// CFileView 对话框

IMPLEMENT_DYNAMIC(CFileView, CDialogEx)

CFileView::CFileView(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialogEx(CFileView::IDD, pParent)
{
	this->m_sock = sock;
	this->pWnd_s = pParent;
}

CFileView::~CFileView()
{
}

void CFileView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CFileView, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CFileView::OnNMClickTree1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CFileView::OnNMRClickList1)
	ON_COMMAND(ID_FILECONTROL_DOWN, &CFileView::OnFilecontrolDown)
	ON_COMMAND(ID_FILE_32774, &CFileView::OnFileUpLoad)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CFileView::OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON_BACK, &CFileView::OnBnClickedButtonBack)
	ON_COMMAND(ID_FILECONTROL_RUNFILE, &CFileView::OnFilecontrolRunfile)
	ON_COMMAND(ID_FILECONTROL_DEL, &CFileView::OnFilecontrolDel)
	ON_COMMAND(ID_FILE_32777, &CFileView::OnFileNewDir)
	ON_COMMAND(ID_FILE_32779, &CFileView::OnFileRefresh)
END_MESSAGE_MAP()


// CFileView 消息处理程序
void CFileView::UpdateMainFrame()
{
	m_imagetree.Create(18, 18, ILC_COLOR32 | ILC_MASK, 1, 1);
	m_imagetree.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON5)));
	m_imagetree.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON6)));
	m_imagetree.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON7)));

	m_tree.SetImageList(&m_imagetree, TVSIL_NORMAL);          //书控件添加图标按钮

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, L"文件名称", LVCFMT_LEFT, 190);
	m_list.InsertColumn(1, L"文件类型", LVCFMT_LEFT, 68);
	m_list.InsertColumn(2, L"文件大小", LVCFMT_LEFT, 100);

	m_imagelist.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 0);
	m_imagelist.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON8)));
	m_imagelist.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON7)));
	m_list.SetImageList(&m_imagelist, LVSIL_SMALL);

	/*m_subbmp[0].LoadBitmapW();
	m_subbmp[0].LoadBitmapW();
	m_subbmp[0].LoadBitmapW();
	m_subbmp[0].LoadBitmapW();
	m_subbmp[0].LoadBitmapW();
	m_subbmp[0].LoadBitmapW();*/

	DirCount = 0;
}


BOOL CFileView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetClientRect(&m_rect);
	MSGINFO msg;
	memset(&msg, 0, sizeof(MSGINFO));
	msg.Msg_id = DISKINFO;
	if (m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg)) == SOCKET_ERROR)
	{
		MessageBox(L"启用磁盘命令发送失败！", L"错误", MB_OK | MB_ICONINFORMATION);
	}
	UpdateMainFrame();
	flag = true;


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CFileView::setDisk(DRIVER disk)
{
	DRIVER *disk_t = new DRIVER;
	CString disk_s;
	int i;
	HTREEITEM ht;
	switch (disk.type)
	{
	case 1:
		disk_s.Format(L"本地磁盘(%c)", disk.disk);
		i = 0;
		break;
	case 2:
		disk_s.Format(L"光驱(%c)", disk.disk);
		i = 1;
		break;
	case 3:
		disk_s.Format(L"可移动磁盘(%c)", disk.disk);
		i = 0;
		break;
	default:
		return;
	}
	ht = m_tree.InsertItem(disk_s, i, i);
	memcpy(disk_t, &disk, sizeof(disk));
	m_tree.SetItemData(ht, (DWORD)disk_t);
}



void CFileView::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	if (flag == false)     //判断点击是否有效
	{
		return;
	}
	//判别点击的是客户区还是节点
	CPoint point(GetMessagePos());
	unsigned int nFlags = 0;
	m_tree.ScreenToClient(&point);
	//HTREEITEM hItem;
	hItem = m_tree.HitTest(point, &nFlags);
	if ((nFlags & TVHT_ONITEM))   //点击的是节点
	{
		GetDlgItem(IDC_EDIT1)->SetWindowTextW(GetPath(hItem));
		m_list.DeleteAllItems();
		DelChilditem(hItem);
		MSGINFO msg;
		msg.Msg_id = FILELIST;
		wchar_t path[260];
		wcscpy_s(path, GetPath(hItem));
		//复制路径到消息结构体
		memcpy(msg.context, (char*)path, sizeof(path));
		m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg));
		flag = false;
	}
	DirCount = 0;
	*pResult = 0;
}

CString CFileView::GetPath(HTREEITEM item)
{
	CString Path;
	CString temp(_T(""));
	while (true)
	{
		HTREEITEM hRoot = m_tree.GetParentItem(item);
		if (hRoot == NULL)
		{
			DRIVER* disk;
			disk = (DRIVER*)m_tree.GetItemData(item);
			Path.Format(_T("%c:"), disk->disk);
			Path = Path + temp;
			break;
			//点击的是磁盘
		}
		else
		{
			temp = _T("\\") + m_tree.GetItemText(item) + temp;
			item = m_tree.GetParentItem(item);
			//点击的是文件，逐级获取即可
		}
	}
	return Path;
}

void CFileView::DelChilditem(HTREEITEM hItem)
{
	while (true)
	{
		if (m_tree.ItemHasChildren(hItem) == TRUE)
		{
			HTREEITEM temp = m_tree.GetChildItem(hItem);
			m_tree.DeleteItem(temp);
		}
		else
		{
			break;
		}
	}
	return;
}

void CFileView::FileList(FILEINFO fileinfo)
{
	switch (fileinfo.type)
	{
	case 1:
	{
			  m_tree.InsertItem(fileinfo.FileName, 2, 2, hItem); //<DIR> m_tree
			  m_list.InsertItem(DirCount, fileinfo.FileName, 1);
			  m_list.SetItemText(DirCount, 1, _T("文件夹"));
			  m_list.SetItemText(DirCount, 2, _T("0"));
			  DirCount++;
	}
		break;
	case 0:
	{
			  int c = m_list.GetItemCount();
			  m_list.InsertItem(c, fileinfo.FileName, 0); //<FILE> m_list
			  __int64 *test = new __int64(fileinfo.size);
			  m_list.SetItemData(c, (DWORD)test);
			  CString ShowSize;
			  ShowSize = GetRealSize(fileinfo.size);
			  m_list.SetItemText(c, 2, ShowSize);
	}
		break;
	case 2:
	{
			  flag = true; //文件获取完成了，可以再次显示文件了
			  m_tree.Expand(hItem, TVE_EXPAND);
			  return;
	}
		break;
	}
	SetShowInfo(hItem);
}

CString CFileView::GetRealSize(__int64 size)
{
	CString si;
	double t;
	if (size >= 1024 * 1024 * 1024)
	{
		t = size * 1.0 / (1024 * 1024 * 1024);
		si.Format(_T("%0.2f GB"), t);
	}
	else if (size >= 1024 * 1024)
	{
		t = size * 1.0 / (1024 * 1024);
		si.Format(_T("%0.2f MB"), t);
	}
	else if (size >= 1024)
	{
		t = size * 1.0 / 1024;
		si.Format(_T("%0.2f KB"), t);
	}
	else
	{
		si.Format(_T("%0.2llu Byte"), size);
	}
	return  si;
}

void CFileView::SetShowInfo(HTREEITEM Item) //显示磁盘及文件信息
{
	CString temp1, temp2;
	if (Item == NULL)
	{
		temp1.Format(_T(""));
		temp2.Format(_T(""));
	}
	else if (m_tree.GetParentItem(Item) == NULL)
	{
		DRIVER* disk;
		disk = (DRIVER*)m_tree.GetItemData(Item);
		switch (disk->type)
		{
		case 1:
			temp1.Format(_T("本地磁盘(%c)\n总大小：%0.2f GB"), disk->disk, disk->all);
			break;
		case 2:
			temp1.Format(_T("光驱(%c)\n总大小：%0.2f GB"), disk->disk, disk->all);
			break;
		case 3:
			temp1.Format(_T("可移动磁盘(%c)\n总大小：%0.2f GB"), disk->disk, disk->all);
			break;
		}
		temp2.Format(_T("可用空间：%0.2f GB"), disk->free);
	}
	else
	{
		int FileCount;
		FileCount = m_list.GetItemCount();
		temp1.Format(_T("%d 个对象"), FileCount);
		temp2.Format(_T(""));
		//显示文件数，文件夹数
	}
	TCHAR *szText1, *szText2;
	szText1 = temp1.GetBuffer(temp1.GetLength());
	szText2 = temp2.GetBuffer(temp2.GetLength());
	::SendMessage(((CFileDlg*)pWnd_s)->m_statusbar, SB_SETTEXT, (WPARAM)0, (LPARAM)szText1);
	::SendMessage(((CFileDlg*)pWnd_s)->m_statusbar, SB_SETTEXT, (WPARAM)1, (LPARAM)szText2);
}


void CFileView::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	CString path1;
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(path1);
	if (path1.IsEmpty())
	{
		return;
	}
	CMenu menu, *pSubMenu;//定义下面要用到的cmenu对象 
	menu.LoadMenu(IDR_MENU2);//装载自定义的右键菜单 
	pSubMenu = menu.GetSubMenu(0);
	/*
	pSubMenu->SetMenuItemBitmaps(0, MF_BYPOSITION, &m_subbmp[0], &m_subbmp[0]);
	pSubMenu->SetMenuItemBitmaps(1, MF_BYPOSITION, &m_subbmp[1], &m_subbmp[1]);
	pSubMenu->SetMenuItemBitmaps(ID_FILECONTROL_DEL, MF_BYCOMMAND, &m_subbmp[2], &m_subbmp[2]);
	pSubMenu->SetMenuItemBitmaps(ID_FILECONTROL_NEWDIR, MF_BYCOMMAND, &m_subbmp[3], &m_subbmp[3]);
	pSubMenu->SetMenuItemBitmaps(ID_FILECONTROL_REFRESH, MF_BYCOMMAND, &m_subbmp[4], &m_subbmp[4]);
	pSubMenu->SetMenuItemBitmaps(ID_FILECONTROL_RUNFILE, MF_BYCOMMAND, &m_subbmp[5], &m_subbmp[5]);
	*/

	if (pNMItemActivate->iItem == -1)
	{
		pSubMenu->EnableMenuItem(ID_FILECONTROL_DOWN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILECONTROL_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED); //del
		pSubMenu->EnableMenuItem(ID_FILECONTROL_RUNFILE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	//获取第一个弹出菜单，所以第一个菜单必须有子菜单 
	CPoint oPoint;//定义一个用于确定光标位置的位置 
	GetCursorPos(&oPoint);//获取当前光标的位置，以便使得菜单可以跟随光标 
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this);
	*pResult = 0;
}

/*
功能：实现点击右键菜单项“下载”的功能
参数：无
返回值：无
*/
void CFileView::OnFilecontrolDown()
{
	// TODO:  在此添加命令处理程序代码
	CCheckDir dir(this);
	dir.DoModal();
	if (dir.flag == true)
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		while (pos)
		{
			int item = m_list.GetNextSelectedItem(pos);
			CString type;
			type = m_list.GetItemText(item, 1);
			if (type.Compare(_T("文件夹")) == 0)
			{
				continue;
			}
			wchar_t filename[120], path1[260], path2[260], size[32];
			//获取源文件路径
			GetDlgItem(IDC_EDIT1)->GetWindowTextW(path1, _countof(path1));
			m_list.GetItemText(item, 0, filename, _countof(filename));
			wcscat_s(path1, _T("\\"));
			wcscat_s(path1, _countof(path1), filename);
			//获取目标路径
			wcscpy_s(path2, _countof(dir.path), dir.path);//===========================================在这里！
			if (_countof(path2) != 3)
			{
				wcscat_s(path2, _T("\\"));
			}
			wcscat_s(path2, _countof(path2), filename);
			//获取文件大小
			m_list.GetItemText(item, 2, size, _countof(size));
			//获取实际大小
			__int64 *usize = (__int64*)(m_list.GetItemData(item));
			((CFileDlg*)pWnd_s)->m_filetrans->OnAddFilelist(0, path1, path2, size, usize);
		}
		((CFileDlg*)pWnd_s)->m_tab.SetCurFocus(1);
	}
}


void CFileView::OnFileUpLoad()
{
	// TODO:  在此添加命令处理程序代码
	CFileDialog dir(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, this, 0, 1);
	if (dir.DoModal() == IDOK)
	{
		CString path1, path2;
		wchar_t size[32];
		wchar_t path_s1[260];
		wchar_t path_s2[260];
		path1 = dir.GetPathName();
		GetDlgItem(IDC_EDIT1)->GetWindowTextW(path2);
		path2 = path2 + _T("\\") + dir.GetFileName();
		WIN32_FIND_DATAW FileData;
		HANDLE h = FindFirstFileW(path1, &FileData);
		if (h == INVALID_HANDLE_VALUE)
		{
			MessageBox(_T("无法传送该文件"), _T("提示"), MB_OK | MB_ICONWARNING);
		}
		else
		{
			DWORDLONG dl;
			dl = MAXDWORD;
			__int64 usize = ((__int64)FileData.nFileSizeHigh * (dl + 1)) + (__int64)FileData.nFileSizeLow;
			CString size_s = GetRealSize(usize);
			wcscpy_s(path_s1, CT2CW(path1));
			wcscpy_s(path_s2, CT2CW(path2));
			wcscpy_s(size, CT2CW(size_s));
			((CFileDlg*)pWnd_s)->m_tab.SetCurFocus(1);
			((CFileDlg*)pWnd_s)->m_filetrans->OnAddFilelist(1, path_s1, path_s2, size, 0);
		}
	}
}


void CFileView::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	if (m_list.GetSelectedCount() == 1)
	{
		/*
		POSITION pos_s;
		pos_s=m_list.GetFirstSelectedItemPosition();
		int item=m_list.GetNextSelectedItem(pos_s);
		__int64 *test = (__int64*)(m_list.GetItemData(item));
		CString t;
		t.Format(_T("%llu"),*test);
		MessageBox(t);
		*/
		POSITION pos_s;
		pos_s = m_list.GetFirstSelectedItemPosition();
		int item = m_list.GetNextSelectedItem(pos_s);
		if (m_list.GetItemText(item, 1).Compare(_T("文件夹")) == 0)
		{
			CString t;
			GetDlgItem(IDC_EDIT1)->GetWindowTextW(t);
			t = t + _T("\\") + m_list.GetItemText(item, 0);
			hItem = GetSelectTreeItem(t);

			GetDlgItem(IDC_EDIT1)->SetWindowTextW(GetPath(hItem));
			m_list.DeleteAllItems();
			DelChilditem(hItem);
			MSGINFO msg;
			msg.Msg_id = FILELIST;
			wchar_t path[260];
			wcscpy_s(path, GetPath(hItem));
			//复制路径到消息结构体
			memcpy(msg.context, (char*)path, sizeof(path));
			m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg));
			flag = false;
			DirCount = 0;
		}
		else
		{
			return;
		}
	}

	*pResult = 0;
}
void CFileView::OnBnClickedButtonBack()
{
	// TODO:  在此添加控件通知处理程序代码
	CString Path;
	CString prePath;
	int pos;
	this->GetDlgItem(IDC_EDIT1)->GetWindowTextW(Path);
	pos = forTransform.findStringPos(Path);
	if (pos == 0)
		return;
	prePath = Path.Left(pos);
	hItem = GetSelectTreeItem(prePath);

	GetDlgItem(IDC_EDIT1)->SetWindowTextW(GetPath(hItem));
	m_list.DeleteAllItems();
	DelChilditem(hItem);

	MSGINFO msg;
	msg.Msg_id = FILELIST;
	wchar_t path[260];
	wcscpy_s(path, GetPath(hItem));
	//复制路径到消息结构体
	memcpy(msg.context, (char*)path, sizeof(path));
	m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg));
	flag = false;
	DirCount = 0;
}

HTREEITEM CFileView::GetSelectTreeItem(CString t) //双击文件夹获取树节点
{
	CString t2 = t.Left(1);
	HTREEITEM tree = m_tree.GetRootItem();
	while (true)
	{
		DRIVER *disk = (DRIVER*)m_tree.GetItemData(tree);
		CString t1;
		t1.Format(_T("%c"), disk->disk);
		if (t2.Compare(t1) == 0)
		{
			break;
		}
		else
		{
			tree = m_tree.GetNextSiblingItem(tree);
		}
	}
	//首先获取盘符，然后与盘符比较获取根级节点
	if (t.GetLength() == 2)
	{
		return tree;//返回刚刚获取的根级节点
	}
	//比较完判断长度
	t = t.Right(t.GetLength() - 3);
	tree = m_tree.GetChildItem(tree);
	while (true)
	{
		int f = t.Find(_T("\\"), 0);
		if (f == -1) //这是最后一个目录
		{
			while (true)
			{
				if (t.Compare(m_tree.GetItemText(tree)) == 0)
				{
					break;
				}
				else
				{
					tree = m_tree.GetNextSiblingItem(tree);
				}
			}
			break;
		}
		else //不止一个目录
		{
			t2 = t.Left(f);
			while (true)
			{
				if (t2.Compare(m_tree.GetItemText(tree)) == 0)
				{
					break;
				}
				else
				{
					tree = m_tree.GetNextSiblingItem(tree);
				}
			}
			tree = m_tree.GetChildItem(tree);
			t = t.Right(t.GetLength() - t2.GetLength() - 1);
		}
	}
	return tree;
}




void CFileView::OnFilecontrolRunfile()
{
	// TODO:  在此添加命令处理程序代码
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	int item = m_list.GetNextSelectedItem(pos);
	CString type;
	type = m_list.GetItemText(item, 1);
	if (type.Compare(_T("文件夹")) != 0)
	{
		MSGINFO msg;
		memset((void*)&msg, 0, sizeof(MSGINFO));
		msg.Msg_id = RUNEXE;
		wchar_t RunPath[260];
		GetDlgItem(IDC_EDIT1)->GetWindowTextW(RunPath, _countof(RunPath));
		wchar_t filename[160];
		m_list.GetItemText(item, 0, filename, _countof(filename));
		wcscat_s(RunPath, _T("\\"));
		wcscat_s(RunPath, filename);
		memcpy((void*)msg.context, (void*)RunPath, sizeof(RunPath));
		m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg));
	}
}


void CFileView::OnFilecontrolDel()
{
	// TODO:  在此添加命令处理程序代码
	int t = MessageBoxW(_T("确认要删除选中的文件吗？"), _T("删除确认"), MB_YESNO | MB_ICONQUESTION);
	if (t == IDYES)
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		MSGINFO msg;
		FILEINFO fileinfo;
		memset((void*)&msg, 0, sizeof(MSGINFO));
		memset((void*)&fileinfo, 0, sizeof(FILEINFO));
		while (pos)
		{
			msg.Msg_id = DELFILE;
			int item = m_list.GetNextSelectedItem(pos);
			CString type;
			type = m_list.GetItemText(item, 1);
			if (type.Compare(_T("文件夹")) == 0)
			{
				fileinfo.type = 1; //删除的是文件夹
			}
			else
			{
				fileinfo.type = 0; //删除的是文件
			}
			//发送
			GetDlgItem(IDC_EDIT1)->GetWindowTextW(fileinfo.FileName, _countof(fileinfo.FileName));
			wchar_t filename[160];
			m_list.GetItemText(item, 0, filename, _countof(filename));
			wcscat_s(fileinfo.FileName, _T("\\"));
			wcscat_s(fileinfo.FileName, filename);
			memcpy((void*)msg.context, (void*)&fileinfo, sizeof(FILEINFO));
			m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg));
			memset((void*)&msg, 0, sizeof(MSGINFO));
			memset((void*)&fileinfo, 0, sizeof(FILEINFO));
		}
	}
}


void CFileView::OnFileNewDir()
{
	// TODO:  在此添加命令处理程序代码
	wchar_t path[260];
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(path, _countof(path));
	wcscat_s(path, _T("\\"));
	CNewDir dlg(this, m_sock, 0, path);
	dlg.DoModal();
}


void CFileView::OnFileRefresh()
{
	// TODO:  在此添加命令处理程序代码
	DelChilditem(hItem);
	m_list.DeleteAllItems();
	MSGINFO msg;
	msg.Msg_id = FILELIST;
	wchar_t Path[MAX_PATH];
	CString path;
	path = this->GetPath(this->hItem);
	lstrcpy(Path, path);
	memcpy(msg.context, (char*)Path, sizeof(Path));
	m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg));


}
