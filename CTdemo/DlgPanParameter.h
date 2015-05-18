#pragma once
#include "afxwin.h"


// CDlgPanParameter �Ի���

class CDlgPanParameter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPanParameter)

public:
	CDlgPanParameter(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPanParameter();

// �Ի�������
	enum { IDD = IDD_PAN_SCAN_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditAngleRange;
	float m_fAngleRange;
	CEdit m_EditRaysNum;
	int m_nRaysNum;
	CEdit m_EditSubPixel;
	float m_fSubPixel;
	CEdit m_EditAnglesNum;
	int m_nAnglesNum;
	CEdit m_EditPanSOR;
	float m_fPanSOR;
	CEdit m_EditPanSOD;
	float m_fPanSOD;
};
