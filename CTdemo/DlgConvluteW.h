#pragma once
#include "afxwin.h"


// CDlgConvluteW 对话框

class CDlgConvluteW : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConvluteW)

public:
	CDlgConvluteW(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgConvluteW();

// 对话框数据
	enum { IDD = IDD_CONVOLUTE_W };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
