#pragma once



// CNewView ��ͼ

class CNewView : public CScrollView
{
	DECLARE_DYNCREATE(CNewView)

protected:
	CNewView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
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
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
	virtual void OnInitialUpdate();     // �����ĵ�һ��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};