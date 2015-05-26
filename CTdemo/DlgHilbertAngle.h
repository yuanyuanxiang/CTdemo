#pragma once
#include "afxwin.h"


// CDlgHilbertAngle 对话框

class CDlgHilbertAngle : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHilbertAngle)

public:
	CDlgHilbertAngle(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgHilbertAngle();

// 对话框数据
	enum { IDD = IDD_HILBERT_ANGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditHilbertAngle;
	float m_fHilberAngle;
};
