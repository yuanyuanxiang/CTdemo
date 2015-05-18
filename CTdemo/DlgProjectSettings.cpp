// DlgProjectSettings.cpp : 实现文件
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgProjectSettings.h"
#include "afxdialogex.h"


// CDlgProjectSettings 对话框

IMPLEMENT_DYNAMIC(CDlgProjectSettings, CDialogEx)

CDlgProjectSettings::CDlgProjectSettings(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgProjectSettings::IDD, pParent)
	, m_nRaysNum(0)
	, m_nAnglesNum(0)
	, m_fAngleRange(0)
	, m_fSubPixel(0)
{

}

CDlgProjectSettings::~CDlgProjectSettings()
{
}

void CDlgProjectSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RAYSNUM, m_EditRaysNum);
	DDX_Control(pDX, IDC_EDIT_ANGLESNUM, m_EditAnglesNum);
	DDX_Text(pDX, IDC_EDIT_RAYSNUM, m_nRaysNum);
	DDV_MinMaxInt(pDX, m_nRaysNum, 1, 4096);
	DDX_Text(pDX, IDC_EDIT_ANGLESNUM, m_nAnglesNum);
	DDV_MinMaxInt(pDX, m_nAnglesNum, 1, 4096);
	DDX_Control(pDX, IDC_ANGLE_RANGE, m_EditAngleRange);
	DDX_Text(pDX, IDC_ANGLE_RANGE, m_fAngleRange);
	DDX_Control(pDX, IDC_SUB_PIXEL, m_EditSubPixel);
	DDX_Text(pDX, IDC_SUB_PIXEL, m_fSubPixel);
}


BEGIN_MESSAGE_MAP(CDlgProjectSettings, CDialogEx)
END_MESSAGE_MAP()


// CDlgProjectSettings 消息处理程序


void CDlgProjectSettings::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_fAngleRange <= 0)
	{
		MessageBox(_T("角度范围设置无效。"), _T("警告"), MB_OK | MB_ICONWARNING);
		m_fAngleRange = 0;
	}

	CDialogEx::OnOK();
}
