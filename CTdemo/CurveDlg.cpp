// CurveDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CurveWnd.h"
#include "CurveDlg.h"
#include "afxdialogex.h"

// CCurveDlg �Ի���

IMPLEMENT_DYNAMIC(CCurveDlg, CDialogEx)

CCurveDlg::CCurveDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CCurveDlg::IDD, pParent)
{
	m_pWndCurve = NULL;
}


CCurveDlg::~CCurveDlg()
{
	if (m_pWndCurve != NULL)
		delete m_pWndCurve;
}

void CCurveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCurveDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CCurveDlg ��Ϣ�������

int CCurveDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//���ߴ�
	CRect rect;
	GetClientRect(&rect);
	m_pWndCurve = new CCurveWnd;
	m_pWndCurve->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, this, IDD_WND_CURVE);

	return 0;
}

void CCurveDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_pWndCurve->GetSafeHwnd())
	{
		m_pWndCurve->MoveWindow(0, 0, cx, cy);
	}
}

BOOL CCurveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//���ڴ�С
	int nDlgWidth = 365+18;
	int nDlgHeight = 375+36;
	::SetWindowPos(this->m_hWnd, HWND_TOP, 100, 100, nDlgWidth, nDlgHeight, SWP_SHOWWINDOW);

	return TRUE;
}

BOOL CCurveDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CCurveDlg::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(rect, RGB(240, 240, 240));
}


void CCurveDlg::SetImage(CImage* pImage)
{
	m_pWndCurve->SetImage(pImage);;
}