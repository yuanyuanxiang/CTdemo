// NewView.cpp : ʵ���ļ�
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


// CNewView ��ͼ

void CNewView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: �������ͼ�ĺϼƴ�С
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CNewView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ���ӻ��ƴ���
}


// CNewView ���

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


// CNewView ��Ϣ�������
