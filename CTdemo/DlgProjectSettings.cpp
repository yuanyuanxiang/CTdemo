// DlgProjectSettings.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CTdemo.h"
#include "DlgProjectSettings.h"
#include "afxdialogex.h"


// CDlgProjectSettings �Ի���

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


// CDlgProjectSettings ��Ϣ�������


void CDlgProjectSettings::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	if (m_fAngleRange <= 0)
	{
		MessageBox(_T("�Ƕȷ�Χ������Ч��"), _T("����"), MB_OK | MB_ICONWARNING);
		m_fAngleRange = 0;
	}

	CDialogEx::OnOK();
}
