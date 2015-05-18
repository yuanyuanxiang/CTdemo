
// ChildFrm.cpp : CChildFrame ���ʵ��
//

#include "stdafx.h"
#include "CTdemo.h"

#include "ChildFrm.h"
#include "CTdemoView.h"
#include "CurveView.h"
#include "NewView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
END_MESSAGE_MAP()

// CChildFrame ����/����

CChildFrame::CChildFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸� CREATESTRUCT cs ���޸Ĵ��������ʽ
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame ���

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame ��Ϣ�������


BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if(m_ImageView.CreateStatic(this, 1, 2) == NULL) 
		return FALSE; 

	CRect rc; 
	GetClientRect(&rc);

	int width = rc.Width();
	int height = rc.Height();
	int right_view_width = 128;
	if (right_view_width >= width)
		right_view_width = width / 2;
	// ��CCTdemoView���ӵ�0��0�д�����
	m_ImageView.CreateView(0, 0, RUNTIME_CLASS(CCTdemoView), CSize(width - right_view_width, height), pContext); 

	if(m_SplitterWnd.CreateStatic(&m_ImageView, 2, 1, WS_CHILD | WS_VISIBLE, m_ImageView.IdFromRowCol(0, 1)) == NULL)
		return FALSE;
	
	// ����0��1���ٷֿ�2��1��
	// ��FormView1�����ӵ��ڶ������������0��0��
	m_SplitterWnd.CreateView(0, 0, RUNTIME_CLASS(CCurveView), CSize(right_view_width, 3 * height / 4), pContext);
	// ��FormView2�����ӵ��ڶ������������1��0��
	m_SplitterWnd.CreateView(1, 0, RUNTIME_CLASS(CNewView), CSize(right_view_width, height / 4), pContext);

	return TRUE; 
}
