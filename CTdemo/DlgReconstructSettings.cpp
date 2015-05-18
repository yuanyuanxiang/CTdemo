// DlgReconstructSettings.cpp : 实现文件
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgReconstructSettings.h"
#include "afxdialogex.h"


// CDlgReconstructSettings 对话框

IMPLEMENT_DYNAMIC(CDlgReconstructSettings, CDialogEx)

CDlgReconstructSettings::CDlgReconstructSettings(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgReconstructSettings::IDD, pParent)
	, m_fAnglesRange(3.141592653f)
	, m_fConvoluteW(0.4f)
	, m_nWidth(256)
	, m_nHeight(256)
{

}

CDlgReconstructSettings::~CDlgReconstructSettings()
{
}

void CDlgReconstructSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ANGLE_RANGE, m_EditAnglesRange);
	DDX_Text(pDX, IDC_ANGLE_RANGE, m_fAnglesRange);
	DDX_Control(pDX, IDC_CONVOLUTE_W, m_EditConvoluteW);
	DDX_Text(pDX, IDC_CONVOLUTE_W, m_fConvoluteW);
	DDX_Control(pDX, IDC_WIDTH, m_EditWidth);
	DDX_Text(pDX, IDC_WIDTH, m_nWidth);
	DDV_MinMaxInt(pDX, m_nWidth, 1, 4096);
	DDX_Control(pDX, IDC_HEIGHT, m_EditHeight);
	DDX_Text(pDX, IDC_HEIGHT, m_nHeight);
}


BEGIN_MESSAGE_MAP(CDlgReconstructSettings, CDialogEx)
END_MESSAGE_MAP()


// CDlgReconstructSettings 消息处理程序
