#pragma once
#include "CurveDlg.h"
#include "CTdemoDoc.h"



// CNewView 视图

class CCurveView : public CScrollView
{
	DECLARE_DYNCREATE(CCurveView)

protected:
	CCurveView();           // 动态创建所使用的受保护的构造函数
	virtual ~CCurveView();

public:
	void SetImage(CImage* pImage, BOOL bRePaint = FALSE);// 设置图像指针
	CCurveDlg* m_pCurveDlg;				// 嵌入式对话框
	CCTdemoDoc* GetMainDoc();			// 获取主窗口指针

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual void OnInitialUpdate();     // 构造后的第一次

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


