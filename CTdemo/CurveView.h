#pragma once
#include "CurveDlg.h"
#include "CTdemoDoc.h"



// CNewView ��ͼ

class CCurveView : public CScrollView
{
	DECLARE_DYNCREATE(CCurveView)

protected:
	CCurveView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CCurveView();

public:
	void SetImage(CImage* pImage, BOOL bRePaint = FALSE);// ����ͼ��ָ��
	CCurveDlg* m_pCurveDlg;				// Ƕ��ʽ�Ի���
	CCTdemoDoc* GetMainDoc();			// ��ȡ������ָ��

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
	virtual void OnInitialUpdate();     // �����ĵ�һ��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


