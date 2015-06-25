#pragma once

#include "resource.h"

// CCurveDlg �Ի���

#define IDD_WND_CURVE 1000

class CCurveWnd;

class CCurveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCurveDlg)

public:
	CCurveDlg(CWnd* pParent = NULL);		// ��׼���캯��
	virtual ~CCurveDlg();

public:
	CCurveWnd*				m_pWndCurve;	// ���ߴ���

// �Ի�������
	enum { IDD = IDD_DLG_CURVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	void SetImage(CImage* pImage, BOOL bRePaint = FALSE);	// Ϊ�Ӵ�������ָ��
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};
