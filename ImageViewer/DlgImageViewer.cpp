// DlgImageViewer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ImageViewer.h"
#include "DlgImageViewer.h"
#include "afxdialogex.h"
#include "DlgImageInfomation.h"
#include "DlgHistogram.h"

// CDlgImageViewer �Ի���

IMPLEMENT_DYNAMIC(CDlgImageViewer, CDialogEx)

CDlgImageViewer::CDlgImageViewer(CWnd* pParent) : CDialogEx(CDlgImageViewer::IDD, pParent)
{
	m_pScrollView = NULL;
	m_pImage = NULL;
	m_fZoomRate = 1.f;
	m_nType = 0;
}

CDlgImageViewer::CDlgImageViewer(CImage* pImage, CWnd* pParent) : CDialogEx(CDlgImageViewer::IDD, pParent)
{
	m_pScrollView = NULL;
	m_pImage = pImage;
	m_fZoomRate = 1.f;
	m_nType = 0;
}

template <typename T> void CDlgImageViewer::Initialize(T* ptr, int width, int height, int rowlen)
{
	m_pScrollView = NULL;
	CreateImage(ptr, width, height, rowlen);
	m_fZoomRate = 1.f;
	// ����dll�Ĳ���CImageָ��
	m_nType = 1;
}

CDlgImageViewer::CDlgImageViewer(BYTE* ptr, int width, int height, int rowlen, CWnd* pParent) 
	: CDialogEx(CDlgImageViewer::IDD, pParent)
{
	Initialize(ptr, width, height, rowlen);
}

CDlgImageViewer::CDlgImageViewer(int* ptr, int width, int height, int rowlen, CWnd* pParent) 
	: CDialogEx(CDlgImageViewer::IDD, pParent)
{
	Initialize(ptr, width, height, rowlen);
}

CDlgImageViewer::CDlgImageViewer(float* ptr, int width, int height, int rowlen, CWnd* pParent) 
	: CDialogEx(CDlgImageViewer::IDD, pParent)
{
	Initialize(ptr, width, height, rowlen);
}

CDlgImageViewer::CDlgImageViewer(double* ptr, int width, int height, int rowlen, CWnd* pParent) 
	: CDialogEx(CDlgImageViewer::IDD, pParent)
{
	Initialize(ptr, width, height, rowlen);
}

CDlgImageViewer::~CDlgImageViewer()
{
}


template <typename T> float CDlgImageViewer::FindMaxData(T* ptr, int width, int height)
{
	if (sizeof(T) == 1) return 255.f;
	float MAX = *ptr;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (*(ptr + j + i * width) > MAX)
			{
				MAX = *(ptr + j + i * width);
			}
		}
	}
	return MAX;
}


template <typename T> float CDlgImageViewer::FindMinData(T* ptr, int width, int height)
{
	if (sizeof(T) == 1) return 0.f;
	float MIN = *ptr;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (*(ptr + j + i * width) < MIN)
			{
				MIN = *(ptr + j + i * width);
			}
		}
	}
	return MIN;
}


template <typename T> void CDlgImageViewer::CopyData(BYTE* pDst, T* ptr, int width, int height, int rowlen)
{
	float MAX = FindMaxData(ptr, width, height);
	float MIN = FindMinData(ptr, width, height);
	if (MAX == MIN) return;
	int ImageRowlen = abs( m_pImage->GetPitch() );
	int channel = m_pImage->GetBPP() / 8;
	if (channel == 1)
	{
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				pDst[j + i * ImageRowlen] = BYTE( 255 * (ptr[j + i * rowlen] - MIN) / (MAX - MIN) );
			}
		}
		return;
	}
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			for (int c = 0; c < channel; ++c)
				pDst[j * channel + c + i * ImageRowlen] = BYTE( 255.f * (ptr[j * channel + c + i * rowlen] - MIN) / (MAX - MIN) );
		}
	}
	return;
}


template <typename T> void CDlgImageViewer::CreateImage(T* ptr, int width, int height, int rowlen)
{
	int channel = rowlen / width;
	int bpp = channel * 8;
	m_pImage = new CImage;
	m_pImage->Create(width, height, bpp, 0UL);
	SetColorTableForImage();
	BYTE* head = GetImageHeadAddress();
	CopyData(head, ptr, width, height, rowlen);
}


void  CDlgImageViewer::SetColorTableForImage()
{
	if (m_pImage == NULL)
		return;
	if (m_pImage->GetBPP() != 8)
		return;
	RGBQUAD ColorTab[256];
	for(int i = 0; i<256; i++)
	{
		ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;
	}
	m_pImage->SetColorTable(0, 256, ColorTab);
}


void  CDlgImageViewer::SetImage(CImage *pImage)
{
	m_pScrollView->m_pImage = pImage;
	CHECK_NULL(pImage);
	m_pScrollView->SetPaintRect(0, 0, pImage->GetWidth(), pImage->GetHeight());
}

void CDlgImageViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgImageViewer, CDialogEx)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_VIEW_ZOOM_IN, &CDlgImageViewer::OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, &CDlgImageViewer::OnViewZoomOut)
	ON_COMMAND(ID_VIEW_DEFAULT, &CDlgImageViewer::OnViewZoomDefault)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN, &CDlgImageViewer::OnUpdateViewZoomIn)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_OUT, &CDlgImageViewer::OnUpdateViewZoomOut)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEFAULT, &CDlgImageViewer::OnUpdateViewZoomDefault)
	ON_COMMAND(ID_FLIP_VERTICAL, &CDlgImageViewer::OnFlipVertical)
	ON_COMMAND(ID_FLIP_HORIZONTAL, &CDlgImageViewer::OnFlipHorizontal)
	ON_COMMAND(ID_IMAGE_INFO, &CDlgImageViewer::OnImageInfo)
	ON_COMMAND(ID_IMAGE_HIST, &CDlgImageViewer::OnImageHist)
	ON_COMMAND(ID_COPY_IMAGE, &CDlgImageViewer::OnCopyImage)
	ON_COMMAND(ID_SAVE_IMAGE, &CDlgImageViewer::OnSaveImage)
	ON_COMMAND(ID_ABOUT, &CDlgImageViewer::OnAbout)
	ON_COMMAND(ID_QUIT, &CDlgImageViewer::OnQuit)
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xffff, OnToolTipText)
END_MESSAGE_MAP()


// CDlgImageViewer ��Ϣ�������


BOOL CDlgImageViewer::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create my toolbar.\n");
		return -1;	    // fail to create
	}

	// ��ImageList�м����Լ���Ҫ��ͼ��
#define ITEMS 15  // ��������ͼ����Ŀ
	m_ImageList.Create(16, 16, ILC_COLOR24 | ILC_MASK, ITEMS, ITEMS);
	m_ImageList.SetBkColor(::GetSysColor(COLOR_BTNFACE));

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ZOOM_IN));			//�Ŵ�
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ZOOM_OUT));			//��С
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ZOOM_DEFAULT));		//1:1

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLIP_V));				//��ֱ��ת
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLIP_H));				//ˮƽ��ת

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_IMAGE_INFO));			//ͼ����Ϣ
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_HISTOGRAM));			//ֱ��ͼ

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_COPY_IMAGE));			//����ͼ��
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_SAVE_IMAGE));			//����ͼ��

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ABOUT));				//����
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_QUIT));				//�˳�

	m_ToolBar.GetToolBarCtrl().SetImageList(&m_ImageList); 

	m_ToolBar.SetButtons(NULL, ITEMS);
	m_ToolBar.SetSizes(CSize(23, 22), CSize(16, 16));

	m_ToolBar.SetButtonInfo(0, ID_VIEW_ZOOM_IN, TBBS_BUTTON, 0);
	m_ToolBar.SetButtonInfo(1, ID_VIEW_ZOOM_OUT, TBBS_BUTTON, 1);
	m_ToolBar.SetButtonInfo(2, ID_VIEW_DEFAULT, TBBS_BUTTON, 2);
	m_ToolBar.SetButtonStyle(3, TBBS_SEPARATOR);

	m_ToolBar.SetButtonInfo(4, ID_FLIP_VERTICAL, TBBS_BUTTON, 3);
	m_ToolBar.SetButtonInfo(5, ID_FLIP_HORIZONTAL, TBBS_BUTTON, 4);
	m_ToolBar.SetButtonStyle(6, TBBS_SEPARATOR);

	m_ToolBar.SetButtonInfo(7, ID_IMAGE_INFO, TBBS_BUTTON, 5);
	m_ToolBar.SetButtonInfo(8, ID_IMAGE_HIST, TBBS_BUTTON, 6);
	m_ToolBar.SetButtonStyle(9, TBBS_SEPARATOR);

	m_ToolBar.SetButtonInfo(10, ID_COPY_IMAGE, TBBS_BUTTON, 7);
	m_ToolBar.SetButtonInfo(11, ID_SAVE_IMAGE, TBBS_BUTTON, 8);
	m_ToolBar.SetButtonStyle(12, TBBS_SEPARATOR);

	m_ToolBar.SetButtonInfo(13, ID_ABOUT, TBBS_BUTTON, 9);
	m_ToolBar.SetButtonInfo(14, ID_QUIT, TBBS_BUTTON, 10);

	m_ToolBar.ShowWindow(SWP_SHOWWINDOW);

	CRect rect;
	GetClientRect(&rect);
	if (m_pScrollView == NULL)
	{
		m_pScrollView = new CImageScrollView;
		m_pScrollView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, this, AFX_IDW_PANE_LAST);
		m_pScrollView->SetScrollSizes(MM_TEXT, CSize(100, 100));
		SetImage(m_pImage);
	}
	// ���ù������ͻ�ͼ���ڵ�λ��
	m_ToolBar.MoveWindow(0, 0, rect.right, 23);
	m_pScrollView->MoveWindow(0, 24, rect.right, rect.bottom - 24);
	// ���öԻ���ͼ��
	SetIcon(AfxGetApp()->LoadIconW(IDI_DLG_ICON), TRUE);
	::SetWindowPos(GetSafeHwnd(), HWND_TOP, 0, 0, 800, 600, SWP_SHOWWINDOW);
	this->CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgImageViewer::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	CRect rect;
	GetClientRect(&rect);
	if(m_ToolBar.GetSafeHwnd())
		m_ToolBar.MoveWindow(0, 0, rect.right, 23);
	if(m_pScrollView->GetSafeHwnd())
		m_pScrollView->MoveWindow(0, 24, rect.right, rect.bottom - 24);
}

void CDlgImageViewer::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	// �����ڴ˴�Destroy����������ڴ�й¶
	if (m_pScrollView != NULL)
	{
		m_pScrollView->DestroyWindow();
		m_pScrollView = NULL;
	}
	// ���m_pImage�ǵ�ǰdll������
	if (m_nType != 0)
	{
		SAFE_DELETE(m_pImage);
	}
}

BOOL CDlgImageViewer::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect rect;
	GetClientRect(&rect);

	// �������ʹ���֮����һ����϶��
	CRect filled(0, 23, rect.right, 24);
	pDC->FillSolidRect(&filled, RGB(255,255,255));
	return TRUE;
}


void CDlgImageViewer::OnViewZoomIn()
{
	CHECK_NULL(m_pImage);
	m_fZoomRate *= 2.f;
	if (m_fZoomRate > 128.f)
		m_fZoomRate = 128.f;
	if (!m_pScrollView->ZoomPaintRect(m_fZoomRate, m_fZoomRate))
		m_fZoomRate /= 2.f;
}


void CDlgImageViewer::OnUpdateViewZoomIn(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_fZoomRate > 1.f);
}


void CDlgImageViewer::OnViewZoomOut()
{
	CHECK_NULL(m_pImage);
	m_fZoomRate /= 2.f;
	if (m_fZoomRate < 1 / 128.f)
		m_fZoomRate = 1 / 128.f;
	if (!m_pScrollView->ZoomPaintRect(m_fZoomRate, m_fZoomRate))
		m_fZoomRate *= 2.f;
}


void CDlgImageViewer::OnUpdateViewZoomOut(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_fZoomRate < 1.f);
}


void CDlgImageViewer::OnViewZoomDefault()
{
	CHECK_NULL(m_pImage);
	m_fZoomRate = 1.f;
	m_pScrollView->ZoomPaintRect(m_fZoomRate, m_fZoomRate);
}


void CDlgImageViewer::OnUpdateViewZoomDefault(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_fZoomRate == 1.f);
}


void CDlgImageViewer::GetImageInfomation(int &width, int &height)
{
	width = m_pImage->GetWidth();
	height = m_pImage->GetHeight();
}


void CDlgImageViewer::GetImageInfomation(int &width, int &height, int &rowlen)
{
	width = m_pImage->GetWidth();
	height = m_pImage->GetHeight();
	rowlen = abs( m_pImage->GetPitch() );
}


void CDlgImageViewer::GetImageInfomation(int &width, int &height, int &rowlen, int &bpp)
{
	width = m_pImage->GetWidth();
	height = m_pImage->GetHeight();
	rowlen = abs( m_pImage->GetPitch() );
	bpp = m_pImage->GetBPP();
}

// ��ȡͼ��Ļ�����Ϣ
/*
int &width		���
int &height		�߶�
int &rowlen		ÿ���ֽ���
int &bpp		λ���
int &channel	ͨ������
*/
void CDlgImageViewer::GetImageInfomation(int &width, int &height, int &rowlen, int &bpp, int &channel)
{
	width = m_pImage->GetWidth();
	height = m_pImage->GetHeight();
	rowlen = abs( m_pImage->GetPitch() );
	bpp = m_pImage->GetBPP();
	channel = bpp / 8;
}

void CDlgImageViewer::OnFlipVertical()
{
	CHECK_NULL(m_pImage);
	int width, height, rowlen, bpp, channel;
	GetImageInfomation(width, height, rowlen, bpp, channel);
	BYTE* temp = new BYTE[height * rowlen];
	BYTE* head = GetImageHeadAddress();
	memcpy(temp, head, height * rowlen);
	for (int r = 0; r < height; ++r)
	{
		memcpy(head + r * rowlen, temp + (height - 1 - r) * rowlen, rowlen);
	}
	SAFE_DELETE(temp);
	m_pScrollView->Invalidate(TRUE);
}


void CDlgImageViewer::OnFlipHorizontal()
{
	CHECK_NULL(m_pImage);
	int width, height, rowlen, bpp, channel;
	GetImageInfomation(width, height, rowlen, bpp, channel);
	BYTE* temp = new BYTE[height * rowlen];
	BYTE* head = GetImageHeadAddress();
	memcpy(temp, head, height * rowlen);
	for (int r = 0; r < height; ++r)
	{
		for (int c = 0; c < width; ++c)
		{
			memcpy(head + c * channel + r * rowlen, temp + ( width - 1 - c ) * channel + r * rowlen, channel);
		}
	}
	SAFE_DELETE(temp);
	m_pScrollView->Invalidate(TRUE);
}


void CDlgImageViewer::OnImageInfo()
{
	CDlgImageInfomation dlg(this);
	dlg.SetImage(m_pImage);
	dlg.DoModal();
}


void CDlgImageViewer::OnImageHist()
{
	CDlgHistogram dlg;
	dlg.SetImage(m_pImage);
	dlg.DoModal();
}


void CDlgImageViewer::OnCopyImage()
{
	m_pScrollView->CopySelectedImage();
}


void CDlgImageViewer::OnSaveImage()
{
	// ������
	CString strFilter = L"����ͼ��|*.BMP;*.DIB;*.RLE;*.JPG;*.JPEG;*.JPE;*.JFIF;*.GIF;*.TIF;*.TIFF;*.PNG;*.ICO|BMP (*.BMP;*.DIB;*.RLE)|*.BMP;*.DIB;*.RLE|JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF)|*.JPG;*.JPEG;*.JPE;*.JFIF|GIF (*.GIF)|*.GIF|ͼ�� (*.ICO)|*.ICO|�����ļ�|*.*||";

	// ��ȡϵͳʱ��
	SYSTEMTIME CurTime;
	CString strDate, strTime;
	GetLocalTime(&CurTime);
	strDate.Format(_T("%4d��%2d��%2d��"), CurTime.wYear, CurTime.wMonth, CurTime.wDay);
	strTime.Format(_T("%2dʱ%2d��%2d��"), CurTime.wHour, CurTime.wMinute, CurTime.wSecond);

	// �ļ��Ի���
	CFileDialog hFileDlg(FALSE, _T(".BMP"), strDate + strTime, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_READONLY, strFilter, NULL);
	hFileDlg.m_ofn.nFilterIndex = 2;
	hFileDlg.m_ofn.lStructSize = sizeof(OPENFILENAME);
	hFileDlg.m_ofn.lpstrTitle = TEXT("����ѡ��ͼ��\0");
	hFileDlg.m_ofn.nMaxFile = MAX_PATH;
	if (hFileDlg.DoModal() != IDOK) return;

	// ��ȡ����·��
	CString file = hFileDlg.GetFileName();
	CString path = hFileDlg.GetPathName();
	CString ext = hFileDlg.GetFileExt();
	if (file == _T(""))
		path = path + strDate + strTime;
	if (ext == _T(""))
	{
		switch (hFileDlg.m_ofn.nFilterIndex)
		{
		case 1 :ext = _T(".BMP"); break;
		case 2 :ext = _T(".BMP"); break;
		case 3 :ext = _T(".JPG"); break;
		case 4 :ext = _T(".GIF"); break;
		case 5 :ext = _T(".ICO"); break;
		case 6 :ext = _T(".JPG"); break;
		default:ext = _T(".JPG"); break;
		}
		path = path + ext;
	}
	m_pScrollView->SaveSelectedImage(path);
}


void CDlgImageViewer::OnAbout()
{
	MessageBox(_T("�˶�̬���ӿ�\"ImageViewer.dll\"\n���׶�ʦ����ѧԬ���鷢����"), _T("����"), MB_OK | MB_ICONINFORMATION);
}


void CDlgImageViewer::OnQuit()
{
	::SendMessage(this->GetSafeHwnd(), WM_CLOSE, 0, 0);
}


BYTE* CDlgImageViewer::GetImageHeadAddress()
{
	return GetImageHeadAddress(m_pImage);
}


BYTE* CDlgImageViewer::GetImageHeadAddress(CImage *pImage)
{
	if (pImage == NULL)
		return NULL;
	return (BYTE *)pImage->GetBits() + pImage->GetPitch() * ( pImage->GetHeight() - 1 );
}


BYTE* CDlgImageViewer::GetImageLineAddress(int LineID)
{
	BYTE *temp = GetImageHeadAddress();
	if ( temp == NULL )
		return NULL;
	return temp + LineID * abs( m_pImage->GetPitch() );
}

void CDlgImageViewer::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CDlgImageViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (zDelta > 0)
	{
		OnViewZoomIn();
	}
	else if (zDelta < 0)
	{
		OnViewZoomOut();
	}
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


// �ο���http://www.cnblogs.com/hcfalan/archive/2010/11/24/1886139.html
BOOL CDlgImageViewer::ContinueModal()
{
	if(m_ToolBar.IsWindowVisible())
	{
		CFrameWnd* pParent = (CFrameWnd*)m_ToolBar.GetParent();
		if(pParent)
			m_ToolBar.OnUpdateCmdUI(pParent, (WPARAM)TRUE);
	}
	CMenu* pMainMenu = GetMenu();
	CCmdUI cmdUI;
	if (pMainMenu != NULL)
	{
		for (UINT n = 0; n < pMainMenu->GetMenuItemCount(); ++n)
		{
			CMenu* pSubMenu = pMainMenu->GetSubMenu(n);
			cmdUI.m_nIndexMax = pSubMenu->GetMenuItemCount();
			for (UINT i = 0; i < cmdUI.m_nIndexMax; ++i)
			{
				cmdUI.m_nIndex = i;
				cmdUI.m_nID = pSubMenu->GetMenuItemID(i);
				cmdUI.m_pMenu = pSubMenu;
				cmdUI.DoUpdate(this, FALSE);
			}
		}
	}
	return CDialogEx::ContinueModal();
}

BOOL CDlgImageViewer::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


// ������ʾ�����ο���http://www.itnose.net/detail/6129398.html
BOOL CDlgImageViewer::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	TOOLTIPTEXT *pTTT=(TOOLTIPTEXT*)pNMHDR;
	// �൱��ԭWM_COMMAND���ݷ�ʽ��wParam��low-order��, ��wParam�зŵ����ǿؼ���ID��
	UINT  uID=pNMHDR->idFrom;

	if(pTTT->uFlags & TTF_IDISHWND)
		uID = ::GetDlgCtrlID((HWND)uID);
	if(uID == NULL)
		return FALSE;
	switch(uID)
	{
	case ID_VIEW_ZOOM_IN:
		pTTT->lpszText = _T("�Ŵ�ͼ����ʾ");
		break;
	case ID_VIEW_ZOOM_OUT:
		pTTT->lpszText = _T("��Сͼ����ʾ");
		break;
	case ID_VIEW_DEFAULT:
		pTTT->lpszText = _T("ͼ��ʵ�ʴ�С");
		break;
	case ID_FLIP_HORIZONTAL:
		pTTT->lpszText = _T("ˮƽ��תͼ��");
		break;
	case ID_FLIP_VERTICAL:
		pTTT->lpszText = _T("��ֱ��תͼ��");
		break;
	case ID_IMAGE_INFO:
		pTTT->lpszText = _T("�鿴ͼ����Ϣ");
		break;
	case ID_IMAGE_HIST:
		pTTT->lpszText = _T("�鿴�Ҷ�ֱ��ͼ");
		break;
	case ID_COPY_IMAGE:
		pTTT->lpszText = _T("����ѡ��ͼ��");
		break;
	case ID_SAVE_IMAGE:
		pTTT->lpszText = _T("����ѡ��ͼ��");
		break;
	case ID_ABOUT:
		pTTT->lpszText = _T("���ڴ˶�̬���ӿ�");
		break;
	case ID_QUIT:
		pTTT->lpszText = _T("�˳�ͼ��Ԥ������");
		break;
	}
	return TRUE;
}