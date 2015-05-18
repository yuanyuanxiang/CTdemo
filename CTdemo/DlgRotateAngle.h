#pragma once
#include "afxwin.h"


// CDlgRotateAngle 对话框

class CDlgRotateAngle : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRotateAngle)

public:
	CDlgRotateAngle(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRotateAngle();

// 对话框数据
	enum { IDD = IDD_ROTATE_ANGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditRotateAngle;
	float m_fRotateAngle;
};
