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
};
