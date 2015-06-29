// DlgRawDataSize.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgRawDataSize.h"
#include "afxdialogex.h"


// CDlgRawDataSize �Ի���

IMPLEMENT_DYNAMIC(CDlgRawDataSize, CDialogEx)

CDlgRawDataSize::CDlgRawDataSize(BOOL bRead, CWnd* pParent) : CDialogEx(CDlgRawDataSize::IDD, pParent)
{
	m_nRawWidth = 1224;
	m_nRawHeight = 720;
	m_nRawHeader = 10816;
	// ����Ǳ����raw�ļ��������߶Ȳ��ɸ���
	m_bReadRaw = bRead;
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
	DDV_MinMaxInt(pDX, m_nRawHeader, 0, 10816);
	DDX_Control(pDX, IDC_RAW_WIDTH, m_EditRawWidth);
	DDX_Control(pDX, IDC_RAW_HEIGHT, m_EditRawHeight);
	DDX_Control(pDX, IDC_RAW_HEADER, m_EditRawHeader);
}


BEGIN_MESSAGE_MAP(CDlgRawDataSize, CDialogEx)
END_MESSAGE_MAP()


// CDlgRawDataSize ��Ϣ�������


BOOL CDlgRawDataSize::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (m_bReadRaw == FALSE)
	{
		m_nRawHeader = -1;
		m_EditRawWidth.EnableWindow(FALSE);
		m_EditRawHeight.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}
