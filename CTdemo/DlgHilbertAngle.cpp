// DlgHilbertAngle.cpp : 实现文件
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgHilbertAngle.h"
#include "afxdialogex.h"


// CDlgHilbertAngle 对话框

IMPLEMENT_DYNAMIC(CDlgHilbertAngle, CDialogEx)

CDlgHilbertAngle::CDlgHilbertAngle(CWnd* pParent /*=NULL*/) : CDialogEx(CDlgHilbertAngle::IDD, pParent)
{
	m_fHilberAngle = 90.f;
}

CDlgHilbertAngle::~CDlgHilbertAngle()
{
}

void CDlgHilbertAngle::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HILBERT_ANGLE, m_EditHilbertAngle);
	DDX_Text(pDX, IDC_HILBERT_ANGLE, m_fHilberAngle);
}


BEGIN_MESSAGE_MAP(CDlgHilbertAngle, CDialogEx)
END_MESSAGE_MAP()


// CDlgHilbertAngle 消息处理程序
