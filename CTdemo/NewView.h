#pragma once



// CNewView 视图

class CNewView : public CScrollView
{
	DECLARE_DYNCREATE(CNewView)

protected:
	CNewView();           // 动态创建所使用的受保护的构造函数
	virtual ~CNewView();

	CRect m_ClientRect;

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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};