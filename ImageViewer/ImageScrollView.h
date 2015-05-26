#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include "DlgQuickViewer.h"


// CImageScrollView ��ͼ

#define HIDE_WINDOW_QUICK_VIEWER 0 // ����Quick Viewer����

class CImageScrollView : public CScrollView
{
	DECLARE_DYNCREATE(CImageScrollView)

public:
	CImage*			m_pImage;				// ָ�򸸴��ڵ�ͼ��ָ��
	CRect			m_PaintRect;			// ��ͼ����
	bool			m_bMovingImage;			// �Ƿ��϶�ͼ��
	bool			m_bPaintRect;			// �Ƿ���ƾ���
	CPoint			m_ptLeftButtonDown;		// ������µĵ�
	CPoint			m_ptLeftButtonUp;		// �������ĵ�
	CPoint			m_ptMoveOrigin;			// ����ƶ����
	CPoint			m_ptMouseMoving;		// ����ƶ���ǰ��
	CToolTipCtrl	m_ToolTipCtrl;			// ��ʾ��

	CDlgQuickViewer*	m_pImageQuickViewer;	// ͼ�����Ԥ��
	bool				m_bRightButtonDown;		// �Ҽ�������
	CPoint				m_ptRightButtonDown;	// �Ҽ����µ�λ��
	CPoint				m_ptRightButtonUp;		// �Ҽ��ͷŵ�λ��
	CRect				m_SourceRect;			// ѡ��Ԥ��������

	CImageScrollView();				// ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CImageScrollView();

	// ���û�ͼ����
	void		SetPaintRect(int left, int top, int right, int bottom);
	bool		CheckZoomAble(float xRate, float yRate);// ����Ƿ��������
	bool		ZoomPaintRect(float xRate, float yRate);// �Ŵ��ͼ����
	void		PaintSinglePoint(CDC* pDC, CPoint &point, int nSize = 3);
	void		PaintSelectedRect(CDC* pDC, CPoint &LeftTop, CPoint &RightBottom);
	void		MovePaintRect(int dx, int dy, CRect &rect);
	void		ShowRGBValue(CPoint &point);			// ��ʾѡ�е��ɫ��
	void		SetToolTipText(CString text);			// ���ù�����ʾ����
	bool		CheckPointInRect(CPoint &point, CRect &rect);
	bool		CheckImageNull();						// ���ͼ���Ƿ�Ϊ��
	void		ShowQuickViewerDlg(int nElapse, int show_type = SW_SHOW);
	void		CopySelectedImage();					// ����ѡ��ͼ��
	void		SaveSelectedImage(CString path);		// ����ѡ��ͼ��
	CPoint		DP2LP(const CPoint &point);				// ��ͼ����ת��Ϊ�ĵ�����
	CRect		DP2LP(const CRect &rect);				// ��ͼ����ת��Ϊ�ĵ�����
	CPoint		DP2LP(CDC* pDC, const CPoint &point);	// ��ͼ����ת��Ϊ�ĵ�����
	CRect		DP2LP(CDC* pDC, const CRect &rect);		// ��ͼ����ת��Ϊ�ĵ�����

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};