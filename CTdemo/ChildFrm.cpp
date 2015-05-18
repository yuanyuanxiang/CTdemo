
// ChildFrm.cpp : CChildFrame 类的实现
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

// CChildFrame 构造/析构

CChildFrame::CChildFrame()
{
	// TODO: 在此添加成员初始化代码
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame 诊断

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

// CChildFrame 消息处理程序


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
	// 将CCTdemoView连接到0行0列窗格上
	m_ImageView.CreateView(0, 0, RUNTIME_CLASS(CCTdemoView), CSize(width - right_view_width, height), pContext); 

	if(m_SplitterWnd.CreateStatic(&m_ImageView, 2, 1, WS_CHILD | WS_VISIBLE, m_ImageView.IdFromRowCol(0, 1)) == NULL)
		return FALSE;
	
	// 将第0行1列再分开2行1列
	// 将FormView1类连接到第二个分栏对象的0行0列
	m_SplitterWnd.CreateView(0, 0, RUNTIME_CLASS(CCurveView), CSize(right_view_width, 3 * height / 4), pContext);
	// 将FormView2类连接到第二个分栏对象的1行0列
	m_SplitterWnd.CreateView(1, 0, RUNTIME_CLASS(CNewView), CSize(right_view_width, height / 4), pContext);

	return TRUE; 
}
