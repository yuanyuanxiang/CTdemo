// ImageQuickViewer.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageViewer.h"
#include "DlgQuickViewer.h"


// CImageQuickViewer

IMPLEMENT_DYNAMIC(CDlgQuickViewer, CDialogEx)

CDlgQuickViewer::CDlgQuickViewer(CWnd* pParent) : CDialogEx(CDlgImageViewer::IDD, pParent)
{
	m_ParentCDC = NULL;
}


CDlgQuickViewer::CDlgQuickViewer(CDC* pDC, CWnd* pParent) : CDialogEx(CDlgImageViewer::IDD, pParent)
{
	m_ParentCDC = pDC;
}


CDlgQuickViewer::~CDlgQuickViewer()
{
}


BEGIN_MESSAGE_MAP(CDlgQuickViewer, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CImageQuickViewer 消息处理程序


void CDlgQuickViewer::SetParentCDC(CDC* pDC)
{
	m_ParentCDC = pDC;
	Invalidate(TRUE);
}


void CDlgQuickViewer::SetSourceRect(CRect rect)
{
	m_SourceRect = rect;
	Invalidate(TRUE);
}


void CDlgQuickViewer::SetSourceRect(int left, int top, int right, int bottom)
{
	m_SourceRect.left = left;
	m_SourceRect.top = top;
	m_SourceRect.right = right;
	m_SourceRect.bottom = bottom;
	Invalidate(TRUE);
}


void CDlgQuickViewer::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	StretchBlt(dc.GetSafeHdc(), 0, 0, m_ClientRect.Width(), m_ClientRect.Height(), 
		m_ParentCDC->GetSafeHdc(), m_SourceRect.left + 1, m_SourceRect.top + 1, m_SourceRect.Width() - 2, m_SourceRect.Height() - 2, SRCCOPY);
}


void CDlgQuickViewer::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	GetClientRect(&m_ClientRect);
}


BOOL CDlgQuickViewer::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


BOOL CDlgQuickViewer::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetClientRect(&m_ClientRect);

	// 设置工具栏和绘图窗口的位置
	::SetWindowPos(GetSafeHwnd(), HWND_TOP, 820, 20, QUICK_VIEWER_SIZE, QUICK_VIEWER_SIZE, SWP_SHOWWINDOW);
	this->CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
}