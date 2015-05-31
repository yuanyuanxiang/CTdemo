// DlgArtSettings.cpp : 实现文件
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgArtSettings.h"
#include "afxdialogex.h"


// CDlgArtSettings 对话框

IMPLEMENT_DYNAMIC(CDlgArtSettings, CDialogEx)

CDlgArtSettings::CDlgArtSettings(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgArtSettings::IDD, pParent)
	, m_nArtItNum(10)
{

}

CDlgArtSettings::~CDlgArtSettings()
{
}

void CDlgArtSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ART_IT_NUM, m_EditArtItNum);
	DDX_Text(pDX, IDC_EDIT_ART_IT_NUM, m_nArtItNum);
	DDV_MinMaxInt(pDX, m_nArtItNum, 1, 9999);
}


BEGIN_MESSAGE_MAP(CDlgArtSettings, CDialogEx)
END_MESSAGE_MAP()


// CDlgArtSettings 消息处理程序
