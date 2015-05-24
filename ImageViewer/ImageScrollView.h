#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include "DlgQuickViewer.h"


// CImageScrollView 视图

#define HIDE_WINDOW_QUICK_VIEWER 0 // 隐藏Quick Viewer窗口

class CImageScrollView : public CScrollView
{
	DECLARE_DYNCREATE(CImageScrollView)

public:
	CImage*			m_pImage;				// 指向父窗口的图像指针
	CRect			m_PaintRect;			// 绘图区域
	bool			m_bMovingImage;			// 是否拖动图像
	bool			m_bPaintRect;			// 是否绘制矩形
	CPoint			m_ptLeftButtonDown;		// 左键按下的点
	CPoint			m_ptLeftButtonUp;		// 左键按起的点
	CPoint			m_ptMoveOrigin;			// 鼠标移动起点
	CPoint			m_ptMouseMoving;		// 鼠标移动当前点
	CToolTipCtrl	m_ToolTipCtrl;			// 提示框

	CDlgQuickViewer*	m_pImageQuickViewer;	// 图像快速预览
	bool				m_bRightButtonDown;		// 右键被按下
	CPoint				m_ptRightButtonDown;	// 右键按下的位置
	CPoint				m_ptRightButtonUp;		// 右键释放的位置
	CRect				m_SourceRect;			// 选择预览的区域

	CImageScrollView();				// 动态创建所使用的受保护的构造函数
	virtual ~CImageScrollView();

	// 设置绘图区域
	void		SetPaintRect(int left, int top, int right, int bottom);
	bool		CheckZoomAble(float xRate, float yRate);// 检测是否可以缩放
	bool		ZoomPaintRect(float xRate, float yRate);// 放大绘图区域
	void		PaintSinglePoint(CDC* pDC, CPoint &point, int nSize = 3);
	void		PaintSelectedRect(CDC* pDC, CPoint &LeftTop, CPoint &RightBottom);
	void		MovePaintRect(int dx, int dy, CRect &rect);
	void		ShowRGBValue(CPoint &point);			// 显示选中点的色彩
	void		SetToolTipText(CString text);			// 设置工具提示文字
	bool		CheckPointInRect(CPoint &point, CRect &rect);
	bool		CheckImageNull();						// 检查图像是否为空
	void		ShowQuickViewerDlg(int nElapse, int show_type = SW_SHOW);
	void		CopySelectedImage();
	void		SaveSelectedImage(CString path);
	CPoint		DP2LP(const CPoint &point);				// 视图坐标转换为文档坐标
	CRect		DP2LP(const CRect &rect);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
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