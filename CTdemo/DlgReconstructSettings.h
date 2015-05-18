#pragma once
#include "afxwin.h"


// CDlgReconstructSettings �Ի���

class CDlgReconstructSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgReconstructSettings)

public:
	CDlgReconstructSettings(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgReconstructSettings();

// �Ի�������
	enum { IDD = IDD_RECONSTRUCT_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditAnglesRange;
	float m_fAnglesRange;
	CEdit m_EditConvoluteW;
	float m_fConvoluteW;
	CEdit m_EditWidth;
	int m_nWidth;
	CEdit m_EditHeight;
	int m_nHeight;
};
