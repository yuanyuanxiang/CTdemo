#pragma once
#include "afxwin.h"


// CDlgConvluteW �Ի���

class CDlgConvluteW : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConvluteW)

public:
	CDlgConvluteW(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgConvluteW();

// �Ի�������
	enum { IDD = IDD_CONVOLUTE_W };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditW;
	float m_fW;
	CStatic m_StaticVar;
	CComboBox m_EditConvKernel;
	int m_nConvKernel;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboConvKernel();
};
