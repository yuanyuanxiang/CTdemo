// DlgImageInfomation.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageViewer.h"
#include "DlgImageInfomation.h"
#include "afxdialogex.h"


// CDlgImageInfomation 对话框

IMPLEMENT_DYNAMIC(CDlgImageInfomation, CDialogEx)

CDlgImageInfomation::CDlgImageInfomation(CWnd* pParent) : CDialogEx(CDlgImageInfomation::IDD, pParent)
{
	m_pImage = NULL;
}

CDlgImageInfomation::~CDlgImageInfomation()
{
}

void CDlgImageInfomation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgImageInfomation, CDialogEx)
END_MESSAGE_MAP()


// CDlgImageInfomation 消息处理程序


void CDlgImageInfomation::SetImage(CImage* pImage)
{
	m_pImage = pImage;
}


void CDlgImageInfomation::ShowInfomation()
{
	if (m_pImage == NULL)
		return;
	CString width, height, rowlen, bpp, channel;
	width.Format(_T("%d"), m_pImage->GetWidth());
	height.Format(_T("%d"), m_pImage->GetHeight());
	rowlen.Format(_T("%d"), abs( m_pImage->GetPitch()));
	bpp.Format(_T("%d"), m_pImage->GetBPP());
	channel.Format(_T("%d"), m_pImage->GetBPP() / 8);
	GetDlgItem(IDC_STATIC_WIDTH)->SetWindowText(width);
	GetDlgItem(IDC_STATIC_HEIGHT)->SetWindowText(height);
	GetDlgItem(IDC_STATIC_ROWLEN)->SetWindowText(rowlen);
	GetDlgItem(IDC_STATIC_CHANNEL)->SetWindowText(channel);
	GetDlgItem(IDC_STATIC_BPP)->SetWindowText(bpp);
}


BOOL CDlgImageInfomation::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ShowInfomation();
	SetIcon(AfxGetApp()->LoadIconW(IDI_IMAGE_INFO), TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
}
