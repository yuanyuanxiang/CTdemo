// DlgRotateAngle.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgRotateAngle.h"
#include "afxdialogex.h"


// CDlgRotateAngle �Ի���

IMPLEMENT_DYNAMIC(CDlgRotateAngle, CDialogEx)

CDlgRotateAngle::CDlgRotateAngle(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRotateAngle::IDD, pParent)
	, m_fRotateAngle(0)
{

}

CDlgRotateAngle::~CDlgRotateAngle()
{
}

void CDlgRotateAngle::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROTATE_ANGLE, m_EditRotateAngle);
	DDX_Text(pDX, IDC_ROTATE_ANGLE, m_fRotateAngle);
}


BEGIN_MESSAGE_MAP(CDlgRotateAngle, CDialogEx)
END_MESSAGE_MAP()


// CDlgRotateAngle ��Ϣ�������
