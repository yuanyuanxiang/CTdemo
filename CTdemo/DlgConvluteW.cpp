// DlgConvluteW.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgConvluteW.h"
#include "afxdialogex.h"


// CDlgConvluteW �Ի���

IMPLEMENT_DYNAMIC(CDlgConvluteW, CDialogEx)

CDlgConvluteW::CDlgConvluteW(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgConvluteW::IDD, pParent)
	, m_fW(0.4f)
{

}

CDlgConvluteW::~CDlgConvluteW()
{
}

void CDlgConvluteW::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditW);
	DDX_Text(pDX, IDC_EDIT1, m_fW);
}


BEGIN_MESSAGE_MAP(CDlgConvluteW, CDialogEx)
END_MESSAGE_MAP()


// CDlgConvluteW ��Ϣ�������
