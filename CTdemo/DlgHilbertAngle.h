#pragma once
#include "afxwin.h"


// CDlgHilbertAngle �Ի���

class CDlgHilbertAngle : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHilbertAngle)

public:
	CDlgHilbertAngle(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgHilbertAngle();

// �Ի�������
	enum { IDD = IDD_HILBERT_ANGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditHilbertAngle;
	float m_fHilberAngle;
};
