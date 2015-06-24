// DlgRawDataSize.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgRawDataSize.h"
#include "afxdialogex.h"


// CDlgRawDataSize 对话框

IMPLEMENT_DYNAMIC(CDlgRawDataSize, CDialogEx)

CDlgRawDataSize::CDlgRawDataSize(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRawDataSize::IDD, pParent)
	, m_nRawWidth(1224)
	, m_nRawHeight(720)
	, m_nRawHeader(10816)
{

}

CDlgRawDataSize::~CDlgRawDataSize()
{
}

void CDlgRawDataSize::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RAW_WIDTH, m_nRawWidth);
	DDV_MinMaxInt(pDX, m_nRawWidth, 0, 9999);
	DDX_Text(pDX, IDC_RAW_HEIGHT, m_nRawHeight);
	DDV_MinMaxInt(pDX, m_nRawHeight, 0, 9999);
	DDX_Text(pDX, IDC_RAW_HEADER, m_nRawHeader);
	DDV_MinMaxInt(pDX, m_nRawHeader, 0, 100000000);
}


BEGIN_MESSAGE_MAP(CDlgRawDataSize, CDialogEx)
END_MESSAGE_MAP()


// CDlgRawDataSize 消息处理程序
