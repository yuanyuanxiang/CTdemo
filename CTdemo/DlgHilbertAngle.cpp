// DlgHilbertAngle.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgHilbertAngle.h"
#include "afxdialogex.h"


// CDlgHilbertAngle �Ի���

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


// CDlgHilbertAngle ��Ϣ�������
