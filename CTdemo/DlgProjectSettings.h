#pragma once
#include "afxwin.h"


// CDlgProjectSettings �Ի���

class CDlgProjectSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProjectSettings)

public:
	CDlgProjectSettings(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProjectSettings();

// �Ի�������
	enum { IDD = IDD_PROJECT_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditRaysNum;
	CEdit m_EditAnglesNum;
	int m_nRaysNum;
	int m_nAnglesNum;
	CEdit m_EditAngleRange;
	float m_fAngleRange;
	virtual void OnOK();
	CEdit m_EditSubPixel;
	float m_fSubPixel;
};
