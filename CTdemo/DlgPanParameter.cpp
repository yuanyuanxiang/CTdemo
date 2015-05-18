// DlgPanParameter.cpp : 实现文件
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgPanParameter.h"
#include "afxdialogex.h"


// CDlgPanParameter 对话框

IMPLEMENT_DYNAMIC(CDlgPanParameter, CDialogEx)

CDlgPanParameter::CDlgPanParameter(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPanParameter::IDD, pParent)
	, m_fAngleRange(0)
	, m_nRaysNum(0)
	, m_fSubPixel(0)
	, m_nAnglesNum(0)
	, m_fPanSOR(0)
	, m_fPanSOD(0)
{

}

CDlgPanParameter::~CDlgPanParameter()
{
}

void CDlgPanParameter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ANGLE_RANGE, m_EditAngleRange);
	DDX_Text(pDX, IDC_ANGLE_RANGE, m_fAngleRange);
	DDX_Control(pDX, IDC_EDIT_RAYSNUM, m_EditRaysNum);
	DDX_Text(pDX, IDC_EDIT_RAYSNUM, m_nRaysNum);
	DDV_MinMaxInt(pDX, m_nRaysNum, 1, 2048);
	DDX_Control(pDX, IDC_SUB_PIXEL, m_EditSubPixel);
	DDX_Text(pDX, IDC_SUB_PIXEL, m_fSubPixel);
	DDX_Control(pDX, IDC_EDIT_ANGLESNUM, m_EditAnglesNum);
	DDX_Text(pDX, IDC_EDIT_ANGLESNUM, m_nAnglesNum);
	DDV_MinMaxInt(pDX, m_nAnglesNum, 1, 2048);
	DDX_Control(pDX, IDC_PAN_SOR, m_EditPanSOR);
	DDX_Text(pDX, IDC_PAN_SOR, m_fPanSOR);
	DDX_Control(pDX, IDC_PAN_SOD, m_EditPanSOD);
	DDX_Text(pDX, IDC_PAN_SOD, m_fPanSOD);
}


BEGIN_MESSAGE_MAP(CDlgPanParameter, CDialogEx)
END_MESSAGE_MAP()


// CDlgPanParameter 消息处理程序
