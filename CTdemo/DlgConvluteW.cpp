// DlgConvluteW.cpp : 实现文件
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgConvluteW.h"
#include "afxdialogex.h"
#include "Functions.h"


// CDlgConvluteW 对话框

IMPLEMENT_DYNAMIC(CDlgConvluteW, CDialogEx)

CDlgConvluteW::CDlgConvluteW(CWnd* pParent /*=NULL*/) : CDialogEx(CDlgConvluteW::IDD, pParent)
, m_nConvKernel(0)
{
	// 实际扫描中，探测器单元尺寸（对第一代CT扫描就是探测器和光机步进的距离）是固定的
	// 即采样间隔是给定的,不妨记其为d。通常假设该采样间隔是足够小的，即d < Tm/2
	// 由此我们可以推导出截止频率 Wm <= 1/(2d)
	m_fW = 0.4;
}

CDlgConvluteW::~CDlgConvluteW()
{
}

void CDlgConvluteW::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditW);
	DDX_Text(pDX, IDC_EDIT1, m_fW);
	DDX_Control(pDX, IDC_VAR, m_StaticVar);
	DDX_Control(pDX, IDC_COMBO_CONV_KERNEL, m_EditConvKernel);
	DDX_CBIndex(pDX, IDC_COMBO_CONV_KERNEL, m_nConvKernel);
}


BEGIN_MESSAGE_MAP(CDlgConvluteW, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_CONV_KERNEL, &CDlgConvluteW::OnCbnSelchangeComboConvKernel)
END_MESSAGE_MAP()


// CDlgConvluteW 消息处理程序


BOOL CDlgConvluteW::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_EditConvKernel.InsertString(CONVOLUTE_KERNEL_COSINE, _T("Cosine窗"));
	m_EditConvKernel.InsertString(CONVOLUTE_KERNEL_RL, _T("R_L窗"));
	m_EditConvKernel.InsertString(CONVOLUTE_KERNEL_SL, _T("S_L窗"));
	m_EditConvKernel.InsertString(CONVOLUTE_KERNEL_HAMMING, _T("Hamming窗"));

	m_EditConvKernel.SetCurSel(CONVOLUTE_KERNEL_COSINE);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgConvluteW::OnCbnSelchangeComboConvKernel()
{
	m_nConvKernel = m_EditConvKernel.GetCurSel();
	switch (m_nConvKernel)
	{
	case CONVOLUTE_KERNEL_COSINE:
		m_fW = 0.4;
		m_EditW.EnableWindow(TRUE);
		m_EditW.SetWindowTextW(_T("0.4"));
		m_StaticVar.SetWindowTextW(_T("w = "));
		break;
	case CONVOLUTE_KERNEL_RL:
		m_EditW.EnableWindow(FALSE);
		m_StaticVar.SetWindowTextW(_T(""));
		break;
	case CONVOLUTE_KERNEL_SL:
		m_EditW.EnableWindow(FALSE);
		m_StaticVar.SetWindowTextW(_T(""));
		break;
	case CONVOLUTE_KERNEL_HAMMING:
		m_fW = 0.1;
		m_EditW.EnableWindow(TRUE);
		m_EditW.SetWindowTextW(_T("0.1"));
		m_StaticVar.SetWindowTextW(_T("a = "));
		break;
	default:
		break;
	}
}
