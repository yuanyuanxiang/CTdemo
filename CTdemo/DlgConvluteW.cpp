// DlgConvluteW.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgConvluteW.h"
#include "afxdialogex.h"
#include "Functions.h"


// CDlgConvluteW �Ի���

IMPLEMENT_DYNAMIC(CDlgConvluteW, CDialogEx)

CDlgConvluteW::CDlgConvluteW(CWnd* pParent /*=NULL*/) : CDialogEx(CDlgConvluteW::IDD, pParent)
, m_nConvKernel(0)
{
	// ʵ��ɨ���У�̽������Ԫ�ߴ磨�Ե�һ��CTɨ�����̽�����͹�������ľ��룩�ǹ̶���
	// ����������Ǹ�����,��������Ϊd��ͨ������ò���������㹻С�ģ���d < Tm/2
	// �ɴ����ǿ����Ƶ�����ֹƵ�� Wm <= 1/(2d)
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


// CDlgConvluteW ��Ϣ�������


BOOL CDlgConvluteW::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_EditConvKernel.InsertString(CONVOLUTE_KERNEL_COSINE, _T("Cosine��"));
	m_EditConvKernel.InsertString(CONVOLUTE_KERNEL_RL, _T("R_L��"));
	m_EditConvKernel.InsertString(CONVOLUTE_KERNEL_SL, _T("S_L��"));
	m_EditConvKernel.InsertString(CONVOLUTE_KERNEL_HAMMING, _T("Hamming��"));

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
