#pragma once

#include "resource.h"

// CCurveDlg 对话框

#define IDD_WND_CURVE 1000

class CCurveWnd;

class CCurveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCurveDlg)

public:
	CCurveDlg(CWnd* pParent = NULL);		// 标准构造函数
	virtual ~CCurveDlg();

public:
	CCurveWnd*				m_pWndCurve;	// 曲线窗口

// 对话框数据
	enum { IDD = IDD_DLG_CURVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void SetImage(CImage* pImage, BOOL bRePaint = FALSE);	// 为子窗口设置指针
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};
