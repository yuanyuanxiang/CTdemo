
// CTdemoView.h : CCTdemoView ��Ľӿ�
//

#pragma once


class CCTdemoDoc;
class CyImage;

// ��ʱ�� 0
#define WATCH_FOR_REPAINT 0

// CPU��������ߴ�
#define CPU_MAX_IMAGE_SIZE 1024

// ���pΪNULL�򷵻�
#define CHECK_NULL(p) 	if ( (p) == NULL ) return;

class CCTdemoView : public CScrollView
{
protected: // �������л�����
	CCTdemoView();
	DECLARE_DYNCREATE(CCTdemoView)

// ����
public:
	CCTdemoDoc* GetDocument() const;

// ����
public:
	int			m_nCurrent;								// ��ǰͼ����
	CyImage*	m_pCurrent;								// ��ǰ��ʾ��ͼ��
	CRect		m_PaintRect;							// ��ͼ����
	CRect		m_ClientRect;							// �ͻ�����
	CRect		m_SelectedRect;							// ��ѡ������
	bool		m_bRePaint;								// �Ƿ��ػ�
	bool		m_bUsingGpu;							// �Ƿ���ü���
	bool		m_bMovingImage;							// �Ƿ��ƶ�ͼ��
	bool		m_bRightButtonDown;						// �Ҽ�������
	HINSTANCE	m_hBppEditorDll;						// ��̬���ӿ�
	float		m_fZoomRate;							// �Ŵ���
	CPoint		m_ptLeftButtonDown;						// ������µĵ�
	CPoint		m_ptLeftButtonUp;						// �������ĵ�
	CPoint		m_ptRightButtonDown;					// �Ҽ����µĵ�
	CPoint		m_ptRightButtonUp;						// �Ҽ�����ĵ�
	CPoint		m_ptMoveOrigin;							// ����ƶ����
	CPoint		m_ptMouseMoving;						// ����ƶ���ǰ��
	int			m_nPointSize;							// ��Ĵ�С
	int			m_nCpuMaxImageSize;						// ��CPU��������ߴ�

	void		RePaint();								// �ػ�ͼ��
	void		PaintSinglePoint(CDC* pDC, CPoint &point, int nSize = 3);
	void		PaintSelectedRect(CDC* pDC, CPoint &LeftTop, CPoint &RightBottom);
	void		ShowRGBValue(CPoint &point);			// ��ʾѡ�е��ɫ��
	bool		CheckPointInRect(CPoint &point, CRect &rect);
	bool		CheckImageNull();						// ���ͼ���Ƿ�Ϊ��
	bool		CheckZoomAble(float xRate, float yRate);// ����Ƿ��������
	void		SetPaintRect(int left, int top, int right, int bottom);
	bool		ZoomPaintRect(float xRate, float yRate);// �Ŵ��ͼ��
	void		MovePaintRect(int dx, int dy, CRect &rect);
	BYTE*		GetImageHeadAddress();					// ��ȡͼ���׵�ַ
	BYTE*		GetImageLineAddress(int LineID);		// ��ȡͼ��ĳ�е�ַ
	void		GetImageInfomation(int &width, int &height, int &rowlen, int &bpp, int &channel);
	void		SetCurrentImage(CyImage* pImage);
	void		SetCurveWndImage(CImage* pImage);

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CCTdemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // CTdemoView.cpp �еĵ��԰汾
inline CCTdemoDoc* CCTdemoView::GetDocument() const
   { return reinterpret_cast<CCTdemoDoc*>(m_pDocument); }
#endif

