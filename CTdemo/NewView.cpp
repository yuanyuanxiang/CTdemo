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
END_MESSAGE_MAP()


// CNewView 绘图

void CNewView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CNewView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
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
