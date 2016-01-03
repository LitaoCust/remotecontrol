// FileView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "FileView.h"
#include "afxdialogex.h"
#include "FileDlg.h"
#include "CheckDir.h"
#include "NewDir.h"



// CFileView �Ի���

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


// CFileView ��Ϣ�������
void CFileView::UpdateMainFrame()
{
	m_imagetree.Create(18, 18, ILC_COLOR32 | ILC_MASK, 1, 1);
	m_imagetree.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON5)));
	m_imagetree.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON6)));
	m_imagetree.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON7)));

	m_tree.SetImageList(&m_imagetree, TVSIL_NORMAL);          //��ؼ����ͼ�갴ť

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, L"�ļ�����", LVCFMT_LEFT, 190);
	m_list.InsertColumn(1, L"�ļ�����", LVCFMT_LEFT, 68);
	m_list.InsertColumn(2, L"�ļ���С", LVCFMT_LEFT, 100);

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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	GetClientRect(&m_rect);
	MSGINFO msg;
	memset(&msg, 0, sizeof(MSGINFO));
	msg.Msg_id = DISKINFO;
	if (m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg)) == SOCKET_ERROR)
	{
		MessageBox(L"���ô��������ʧ�ܣ�", L"����", MB_OK | MB_ICONINFORMATION);
	}
	UpdateMainFrame();
	flag = true;


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
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
		disk_s.Format(L"���ش���(%c)", disk.disk);
		i = 0;
		break;
	case 2:
		disk_s.Format(L"����(%c)", disk.disk);
		i = 1;
		break;
	case 3:
		disk_s.Format(L"���ƶ�����(%c)", disk.disk);
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (flag == false)     //�жϵ���Ƿ���Ч
	{
		return;
	}
	//�б������ǿͻ������ǽڵ�
	CPoint point(GetMessagePos());
	unsigned int nFlags = 0;
	m_tree.ScreenToClient(&point);
	//HTREEITEM hItem;
	hItem = m_tree.HitTest(point, &nFlags);
	if ((nFlags & TVHT_ONITEM))   //������ǽڵ�
	{
		GetDlgItem(IDC_EDIT1)->SetWindowTextW(GetPath(hItem));
		m_list.DeleteAllItems();
		DelChilditem(hItem);
		MSGINFO msg;
		msg.Msg_id = FILELIST;
		wchar_t path[260];
		wcscpy_s(path, GetPath(hItem));
		//����·������Ϣ�ṹ��
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
			//������Ǵ���
		}
		else
		{
			temp = _T("\\") + m_tree.GetItemText(item) + temp;
			item = m_tree.GetParentItem(item);
			//��������ļ����𼶻�ȡ����
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
			  m_list.SetItemText(DirCount, 1, _T("�ļ���"));
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
			  flag = true; //�ļ���ȡ����ˣ������ٴ���ʾ�ļ���
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

void CFileView::SetShowInfo(HTREEITEM Item) //��ʾ���̼��ļ���Ϣ
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
			temp1.Format(_T("���ش���(%c)\n�ܴ�С��%0.2f GB"), disk->disk, disk->all);
			break;
		case 2:
			temp1.Format(_T("����(%c)\n�ܴ�С��%0.2f GB"), disk->disk, disk->all);
			break;
		case 3:
			temp1.Format(_T("���ƶ�����(%c)\n�ܴ�С��%0.2f GB"), disk->disk, disk->all);
			break;
		}
		temp2.Format(_T("���ÿռ䣺%0.2f GB"), disk->free);
	}
	else
	{
		int FileCount;
		FileCount = m_list.GetItemCount();
		temp1.Format(_T("%d ������"), FileCount);
		temp2.Format(_T(""));
		//��ʾ�ļ������ļ�����
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString path1;
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(path1);
	if (path1.IsEmpty())
	{
		return;
	}
	CMenu menu, *pSubMenu;//��������Ҫ�õ���cmenu���� 
	menu.LoadMenu(IDR_MENU2);//װ���Զ�����Ҽ��˵� 
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
	//��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵� 
	CPoint oPoint;//����һ������ȷ�����λ�õ�λ�� 
	GetCursorPos(&oPoint);//��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը����� 
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this);
	*pResult = 0;
}

/*
���ܣ�ʵ�ֵ���Ҽ��˵�����ء��Ĺ���
��������
����ֵ����
*/
void CFileView::OnFilecontrolDown()
{
	// TODO:  �ڴ���������������
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
			if (type.Compare(_T("�ļ���")) == 0)
			{
				continue;
			}
			wchar_t filename[120], path1[260], path2[260], size[32];
			//��ȡԴ�ļ�·��
			GetDlgItem(IDC_EDIT1)->GetWindowTextW(path1, _countof(path1));
			m_list.GetItemText(item, 0, filename, _countof(filename));
			wcscat_s(path1, _T("\\"));
			wcscat_s(path1, _countof(path1), filename);
			//��ȡĿ��·��
			wcscpy_s(path2, _countof(dir.path), dir.path);//===========================================�����
			if (_countof(path2) != 3)
			{
				wcscat_s(path2, _T("\\"));
			}
			wcscat_s(path2, _countof(path2), filename);
			//��ȡ�ļ���С
			m_list.GetItemText(item, 2, size, _countof(size));
			//��ȡʵ�ʴ�С
			__int64 *usize = (__int64*)(m_list.GetItemData(item));
			((CFileDlg*)pWnd_s)->m_filetrans->OnAddFilelist(0, path1, path2, size, usize);
		}
		((CFileDlg*)pWnd_s)->m_tab.SetCurFocus(1);
	}
}


void CFileView::OnFileUpLoad()
{
	// TODO:  �ڴ���������������
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
			MessageBox(_T("�޷����͸��ļ�"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
		if (m_list.GetItemText(item, 1).Compare(_T("�ļ���")) == 0)
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
			//����·������Ϣ�ṹ��
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	//����·������Ϣ�ṹ��
	memcpy(msg.context, (char*)path, sizeof(path));
	m_Mysock.SendCommand(m_sock, (char*)&msg, sizeof(msg));
	flag = false;
	DirCount = 0;
}

HTREEITEM CFileView::GetSelectTreeItem(CString t) //˫���ļ��л�ȡ���ڵ�
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
	//���Ȼ�ȡ�̷���Ȼ�����̷��Ƚϻ�ȡ�����ڵ�
	if (t.GetLength() == 2)
	{
		return tree;//���ظոջ�ȡ�ĸ����ڵ�
	}
	//�Ƚ����жϳ���
	t = t.Right(t.GetLength() - 3);
	tree = m_tree.GetChildItem(tree);
	while (true)
	{
		int f = t.Find(_T("\\"), 0);
		if (f == -1) //�������һ��Ŀ¼
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
		else //��ֹһ��Ŀ¼
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
	// TODO:  �ڴ���������������
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	int item = m_list.GetNextSelectedItem(pos);
	CString type;
	type = m_list.GetItemText(item, 1);
	if (type.Compare(_T("�ļ���")) != 0)
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
	// TODO:  �ڴ���������������
	int t = MessageBoxW(_T("ȷ��Ҫɾ��ѡ�е��ļ���"), _T("ɾ��ȷ��"), MB_YESNO | MB_ICONQUESTION);
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
			if (type.Compare(_T("�ļ���")) == 0)
			{
				fileinfo.type = 1; //ɾ�������ļ���
			}
			else
			{
				fileinfo.type = 0; //ɾ�������ļ�
			}
			//����
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
	// TODO:  �ڴ���������������
	wchar_t path[260];
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(path, _countof(path));
	wcscat_s(path, _T("\\"));
	CNewDir dlg(this, m_sock, 0, path);
	dlg.DoModal();
}


void CFileView::OnFileRefresh()
{
	// TODO:  �ڴ���������������
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
