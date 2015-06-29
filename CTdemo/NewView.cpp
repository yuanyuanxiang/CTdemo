// NewView.cpp : 实现文件
//

#include "stdafx.h"
#include "CTdemo.h"
#include "NewView.h"


// CNewView

IMPLEMENT_DYNCREATE(CNewView, CScrollView)

CNewView::CNewView()
{

}

CNewView::~CNewView()
{
}


BEGIN_MESSAGE_MAP(CNewView, CScrollView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CNewView 绘图

void CNewView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
}

void CNewView::OnDraw(CDC* pDC)
{
	CyImage* pImage = AfxGetImage();
	if (!CHECK_IMAGE_NULL(pImage))
	{
		pImage->Draw(pDC->GetSafeHdc(), 0, 0, pImage->GetWidth(), pImage->GetHeight());
	}
	else
	{
		pDC->FillSolidRect(&m_ClientRect, RGB(240, 240, 240));
	}
}


// CNewView 诊断

#ifdef _DEBUG
void CNewView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CNewView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CNewView 消息处理程序


void CNewView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	GetClientRect(&m_ClientRect);
}


BOOL CNewView::OnEraseBkgnd(CDC* pDC)
{
	// 1	2	3
	// 4	5	6
	// 7	8	9
	// 其中5代表绘图区域
	CyImage* pImage = AfxGetImage();
	if (CHECK_IMAGE_NULL(pImage))
		return CScrollView::OnEraseBkgnd(pDC);
	CRect m_PaintRect(0, 0, pImage->GetWidth(), pImage->GetHeight());
	pDC->PatBlt(m_PaintRect.right, 0, m_ClientRect.right - m_PaintRect.right, m_ClientRect.bottom, PATCOPY);	// 3 6 9	
	pDC->PatBlt(0, m_PaintRect.bottom, m_ClientRect.right, m_ClientRect.bottom - m_PaintRect.bottom, PATCOPY);	// 7 8 9
	return TRUE;
}


int CNewView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetScrollSizes(MM_TEXT, CSize(0, 0));

	return 0;
}
