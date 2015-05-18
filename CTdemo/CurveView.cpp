// NewView.cpp : 实现文件
//

#include "stdafx.h"
#include "CTdemo.h"
#include "CurveView.h"
#include "CTdemoDoc.h"
#include "MainFrm.h"
#include "ChildFrm.h"


// CNewView

IMPLEMENT_DYNCREATE(CCurveView, CScrollView)

CCurveView::CCurveView()
{
	m_pCurveDlg = NULL;
}

CCurveView::~CCurveView()
{
	SAFE_DELETE(m_pCurveDlg);
}


BEGIN_MESSAGE_MAP(CCurveView, CScrollView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CNewView 绘图

void CCurveView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


CCTdemoDoc* CCurveView::GetMainDoc()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CChildFrame* pChildFrame = (CChildFrame*)pMainFrame->GetActiveFrame();
	CCTdemoDoc* pDoc = (CCTdemoDoc*)pChildFrame->GetActiveDocument();
	return pDoc;
}


void CCurveView::OnDraw(CDC* pDC)
{

}


// CNewView 诊断

#ifdef _DEBUG
void CCurveView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CCurveView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CNewView 消息处理程序


int CCurveView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pCurveDlg = new CCurveDlg;
	m_pCurveDlg->Create(IDD_DLG_CURVE, this);

	return 0;
}


void CCurveView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	if (m_pCurveDlg->GetSafeHwnd())
	{
		m_pCurveDlg->MoveWindow(0, 0, cx, cy);
	}
}


BOOL CCurveView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


void CCurveView::SetImage(CImage* pImage)
{
	m_pCurveDlg->SetImage(pImage);
}