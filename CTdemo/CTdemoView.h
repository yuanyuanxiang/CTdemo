
// CTdemoView.h : CCTdemoView 类的接口
//

#pragma once


class CCTdemoDoc;
class CyImage;

// 定时器 0
#define WATCH_FOR_REPAINT 0

// CPU处理的最大尺寸
#define CPU_MAX_IMAGE_SIZE 1024

// 检测p为NULL则返回
#define CHECK_NULL(p) 	if ( (p) == NULL ) return;

class CCTdemoView : public CScrollView
{
protected: // 仅从序列化创建
	CCTdemoView();
	DECLARE_DYNCREATE(CCTdemoView)

// 特性
public:
	CCTdemoDoc* GetDocument() const;

// 操作
public:
	int			m_nCurrent;								// 当前图像标记
	CyImage*	m_pCurrent;								// 当前显示的图像
	CRect		m_PaintRect;							// 绘图区域
	CRect		m_ClientRect;							// 客户区域
	CRect		m_SelectedRect;							// 被选中区域
	bool		m_bRePaint;								// 是否重绘
	bool		m_bUsingGpu;							// 是否采用加速
	bool		m_bMovingImage;							// 是否移动图像
	bool		m_bRightButtonDown;						// 右键被按下
	HINSTANCE	m_hBppEditorDll;						// 动态链接库
	float		m_fZoomRate;							// 放大倍率
	CPoint		m_ptLeftButtonDown;						// 左键按下的点
	CPoint		m_ptLeftButtonUp;						// 左键按起的点
	CPoint		m_ptRightButtonDown;					// 右键按下的点
	CPoint		m_ptRightButtonUp;						// 右键按起的点
	CPoint		m_ptMoveOrigin;							// 鼠标移动起点
	CPoint		m_ptMouseMoving;						// 鼠标移动当前点
	int			m_nPointSize;							// 点的大小
	int			m_nCpuMaxImageSize;						// 用CPU处理的最大尺寸

	void		RePaint();								// 重绘图像
	void		PaintSinglePoint(CDC* pDC, CPoint &point, int nSize = 3);
	void		PaintSelectedRect(CDC* pDC, CPoint &LeftTop, CPoint &RightBottom);
	void		ShowRGBValue(CPoint &point);			// 显示选中点的色彩
	bool		CheckPointInRect(CPoint &point, CRect &rect);
	bool		CheckImageNull();						// 检查图像是否为空
	bool		CheckZoomAble(float xRate, float yRate);// 检查是否可以缩放
	void		SetPaintRect(int left, int top, int right, int bottom);
	bool		ZoomPaintRect(float xRate, float yRate);// 放大绘图区
	void		MovePaintRect(int dx, int dy, CRect &rect);
	BYTE*		GetImageHeadAddress();					// 获取图像首地址
	BYTE*		GetImageLineAddress(int LineID);		// 获取图像某行地址
	void		GetImageInfomation(int &width, int &height, int &rowlen, int &bpp, int &channel);
	void		SetCurrentImage(CyImage* pImage);
	void		SetCurveWndImage(CImage* pImage);

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CCTdemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnToolbarBpp();
	afx_msg void OnUpdateToolbarBpp(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarTrs();
	afx_msg void OnUpdateToolbarTrs(CCmdUI *pCmdUI);
	afx_msg void OnToolbarProject();
	afx_msg void OnUpdateToolbarPrj(CCmdUI *pCmdUI);
	afx_msg void OnProjectSettings();
	afx_msg void OnSaveProjectImg();
	afx_msg void OnUpdateSaveProjectImg(CCmdUI *pCmdUI);
	afx_msg void OnToolbarConvolute();
	afx_msg void OnUpdateToolbarConv(CCmdUI *pCmdUI);
	afx_msg void OnToolbarBackProject();
	afx_msg void OnUpdateToolbarBackProject(CCmdUI *pCmdUI);
	afx_msg void OnSaveBackProjectImg();
	afx_msg void OnUpdateSaveBackProjectImg(CCmdUI *pCmdUI);
	afx_msg void OnSaveAfterFilterImg();
	afx_msg void OnUpdateSaveAfterFilterImg(CCmdUI *pCmdUI);
	afx_msg void OnToolbarPopupImage();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUpdateToolbarPopupImage(CCmdUI *pCmdUI);
	afx_msg void OnConvUsingGpu();
	afx_msg void OnUpdateConvUsingGpu(CCmdUI *pCmdUI);
	afx_msg void OnBkpUsingGpu();
	afx_msg void OnUpdateBkpUsingGpu(CCmdUI *pCmdUI);
	afx_msg void OnToolbarZoomIn();
	afx_msg void OnUpdateToolbarZoomIn(CCmdUI *pCmdUI);
	afx_msg void OnToolbarZoomOut();
	afx_msg void OnUpdateToolbarZoomOut(CCmdUI *pCmdUI);
	afx_msg void OnToolbarZoomDefault();
	afx_msg void OnUpdateToolbarZoomDefault(CCmdUI *pCmdUI);
	afx_msg void OnToolbarFlipH();
	afx_msg void OnUpdateToolbarFlipH(CCmdUI *pCmdUI);
	afx_msg void OnToolbarFlipV();
	afx_msg void OnUpdateToolbarFlipV(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPanProject();
	afx_msg void OnUpdatePanProject(CCmdUI *pCmdUI);
	afx_msg void OnPanReconstruct();
	afx_msg void OnUpdatePanReconstruct(CCmdUI *pCmdUI);
	afx_msg void OnFanScanSettings();
	afx_msg void OnGpuPanProject();
	afx_msg void OnUpdateAmpPanProject(CCmdUI *pCmdUI);
	afx_msg void OnCudaPanProject();
	afx_msg void OnUpdateCudaPanProject(CCmdUI *pCmdUI);
	afx_msg void OnShowAllImage();
	afx_msg void OnChangeImageShow();
};

#ifndef _DEBUG  // CTdemoView.cpp 中的调试版本
inline CCTdemoDoc* CCTdemoView::GetDocument() const
   { return reinterpret_cast<CCTdemoDoc*>(m_pDocument); }
#endif

