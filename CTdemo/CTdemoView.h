
// CTdemoView.h : CCTdemoView ��Ľӿ�
//

#pragma once

// CUDA Runtime, Interop, and includes
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <driver_functions.h>

// CUDA utilities
#include <helper_cuda.h>
#include <helper_cuda_gl.h>

// Helper functions
#include <helper_cuda.h>
#include <helper_functions.h>
#include <helper_timer.h>

class CCTdemoDoc;
class CyImage;

// ��ͼ֮��ͬ��
#define VIEWS_SYNCHRONIZE 0

// CPU��������ߴ�
#define CPU_COMPUTE_MAX_IMAGE_SIZE 1024

// ���ͼ��pΪ�գ�����ǿգ����Ϊ1
#define CHECK_IMAGE_NULL(p) ( ((p) == NULL) || ((p)->IsNull()) )

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned char VolumeType;

#define MAX_EPSILON_ERROR 5.00f
#define THRESHOLD         0.30f

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
	int			m_nCurrent;								// ��ǰͼ���Ǻ�
	CyImage*	m_pCurrent;								// ��ǰ��ʾ��ͼ��
	CRect		m_PaintRect;							// ��ͼ����
	CRect		m_ClientRect;							// �ͻ�����
	CRect		m_SelectedRect;							// ��ѡ�о�������
	bool		m_bRePaint;								// �Ƿ��ػ�ͼ��
	bool		m_bUsingGpu;							// �Ƿ����GPU
	bool		m_bMovingImage;							// �Ƿ��ƶ�ͼ��
	bool		m_bRightButtonDown;						// �Ҽ�������
	HINSTANCE	m_hBppEditorDll;						// BPP��̬���ӿ�
	float		m_fZoomRate;							// ͼ��Ŵ���
	CPoint		m_ptLeftButtonDown;						// ������µĵ�
	CPoint		m_ptLeftButtonUp;						// ����ͷŵĵ�
	CPoint		m_ptRightButtonDown;					// �Ҽ����µĵ�
	CPoint		m_ptRightButtonUp;						// �Ҽ��ͷŵĵ�
	CPoint		m_ptMoveOrigin;							// ����ƶ����
	CPoint		m_ptMouseMoving;						// ����ƶ���ǰ��
	int			m_nPointSize;							// ���Ƶ�Ĵ�С
	int			m_nCpuMaxImageSize;						// ��CPU��������ߴ�

	/*OpenGL*/
	bool		m_bUsingOpenGL;							//ʹ��OpenGL
	int			m_nViewportWidth;						//�ӿڿ��
	int			m_nViewportHeight;						//�ӿڸ߶�
	PIXELFORMATDESCRIPTOR *m_pfd;						//���ظ�ʽ
	int			m_nGLPixelIndex;						//��������
	HGLRC		m_hGLContext;							//Rendering Context��ɫ������
	BOOL		InitializeOpenGL(HDC hDC);				//��ʼ�� OpenGL
	void		RenderScene();							//���Ƴ���
	BOOL		SetWindowPixelFormat(HDC hDC);			//�������ظ�ʽ
	BOOL		CreateViewGLContext(HDC hDC);			//����OpenGL������

	void		RePaint();								// �ػ�ͼ��
	void		PaintSinglePoint(CDC* pDC, CPoint &point, int nSize = 3);
	void		PaintSelectedRect(CDC* pDC, CPoint &LeftTop, CPoint &RightBottom);
	void		ShowRGBValue(CPoint &point);			// ��ʾѡ�е��ɫ��
	bool		CheckPointInRect(CPoint &point, CRect &rect);//�����ھ�����
	bool		CheckZoomAble(float xRate, float yRate);// ����Ƿ��������
	void		SetPaintRect(int left, int top, int right, int bottom);
	bool		ZoomPaintRect(float xRate, float yRate);// �Ŵ��ͼ��
	void		MovePaintRect(int dx, int dy, CRect &rect);// �ƶ���ͼ����
	void		SetCurrentImage(CyImage* pImage);		// ���õ�ǰͼ��
	void		SetCurveWndImage(CImage* pImage);		// ��������ͼ��
	CPoint		DP2LP(const CPoint &point);				// ��ͼ����ת��Ϊ�ĵ�����
	CRect		DP2LP(const CRect &rect);				// ��ͼ����ת��Ϊ�ĵ�����
	CPoint		DP2LP(CDC* pDC, const CPoint &point);	// ��ͼ����ת��Ϊ�ĵ�����
	CRect		DP2LP(CDC* pDC, const CRect &rect);		// ��ͼ����ת��Ϊ�ĵ�����

	void CopyImage(CyImage* pImage);					// ����ͼ��
	void PasteImage();									// ���ͼ��

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
	afx_msg void OnToolbarChangeBpp();
	afx_msg void OnUpdateToolbarChangeBpp(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarTransform();
	afx_msg void OnUpdateToolbarTransform(CCmdUI *pCmdUI);
	afx_msg void OnToolbarProject();
	afx_msg void OnUpdateToolbarProject(CCmdUI *pCmdUI);
	afx_msg void OnProjectSettings();
	afx_msg void OnSaveProjectImg();
	afx_msg void OnUpdateSaveProjectImg(CCmdUI *pCmdUI);
	afx_msg void OnToolbarConvolute();
	afx_msg void OnUpdateToolbarConvolute(CCmdUI *pCmdUI);
	afx_msg void OnToolbarBackProject();
	afx_msg void OnUpdateToolbarBackProject(CCmdUI *pCmdUI);
	afx_msg void OnSaveBackProjectImg();
	afx_msg void OnUpdateSaveBackProjectImg(CCmdUI *pCmdUI);
	afx_msg void OnSaveAfterFilterImg();
	afx_msg void OnUpdateSaveAfterFilterImg(CCmdUI *pCmdUI);
	afx_msg void OnToolbarPopupImage();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUpdateToolbarPopupImage(CCmdUI *pCmdUI);
	afx_msg void OnConvoluteUsingGpu();
	afx_msg void OnUpdateConvoluteUsingGpu(CCmdUI *pCmdUI);
	afx_msg void OnBackProjectUsingGpu();
	afx_msg void OnUpdateBackProjectUsingGpu(CCmdUI *pCmdUI);
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
	afx_msg void OnCudaPanProject();
	afx_msg void OnUpdateCudaPanProject(CCmdUI *pCmdUI);
	afx_msg void OnChangeImageShowNext();
	afx_msg void OnChangeImageShowPrev();
	afx_msg void OnReconstructImageSize();
	afx_msg void OnUpdateReconstructImageSize(CCmdUI *pCmdUI);
	afx_msg void OnDbpImage();
	afx_msg void OnUpdateDbpImage(CCmdUI *pCmdUI);
	afx_msg void OnToolbarInverseHilbert();
	afx_msg void OnUpdateToolbarInverseHilbert(CCmdUI *pCmdUI);
	afx_msg void OnArtMethod();
	afx_msg void OnUpdateArtMethod(CCmdUI *pCmdUI);
	afx_msg void OnArtRadon();
	afx_msg void OnUpdateArtRadon(CCmdUI *pCmdUI);
	afx_msg void OnToolbarOriginImage();
	afx_msg void OnUpdateToolbarOriginImage(CCmdUI *pCmdUI);
	afx_msg void OnToolbarProjectImage();
	afx_msg void OnUpdateToolbarProjectImage(CCmdUI *pCmdUI);
	afx_msg void OnToolbarAfterProjectImage();
	afx_msg void OnUpdateToolbarAfterProjectImage(CCmdUI *pCmdUI);
	afx_msg void OnToolbarReconstructImage();
	afx_msg void OnUpdateToolbarReconstructImage(CCmdUI *pCmdUI);
	afx_msg void OnToolbarImageToProject();
	afx_msg void OnUpdateToolbarImageToProject(CCmdUI *pCmdUI);
	afx_msg void OnToolbarProjectToImage();
	afx_msg void OnUpdateToolbarProjectToImage(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};

#ifndef _DEBUG  // CTdemoView.cpp �еĵ��԰汾
inline CCTdemoDoc* CCTdemoView::GetDocument() const
   { return reinterpret_cast<CCTdemoDoc*>(m_pDocument); }
#endif