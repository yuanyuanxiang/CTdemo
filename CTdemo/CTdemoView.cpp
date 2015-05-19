
// CTdemoView.cpp : CCTdemoView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "CTdemo.h"
#endif

#include "CTdemoDoc.h"
#include "CTdemoView.h"
#include "DlgRotateAngle.h"
#include "DlgProjectSettings.h"
#include "DlgConvluteW.h"
#include "MainFrm.h"
#include <omp.h>
#include "DlgPanParameter.h"
#include "CurveView.h"
#include "DlgReconstructSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����ʾ���ݽضϾ���
#pragma warning(disable: 4244)

// CCTdemoView

IMPLEMENT_DYNCREATE(CCTdemoView, CScrollView)

BEGIN_MESSAGE_MAP(CCTdemoView, CScrollView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCTdemoView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND(ID_TOOLBAR_BPP, &CCTdemoView::OnToolbarChangeBpp)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_BPP, &CCTdemoView::OnUpdateToolbarChangeBpp)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_TRS, &CCTdemoView::OnToolbarTransform)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_TRS, &CCTdemoView::OnUpdateToolbarTransform)
	ON_COMMAND(ID_TOOLBAR_PRJ, &CCTdemoView::OnToolbarProject)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_PRJ, &CCTdemoView::OnUpdateToolbarProject)
	ON_COMMAND(ID_PROJECT_SETTINGS, &CCTdemoView::OnProjectSettings)
	ON_COMMAND(ID_SAVE_PROJECT, &CCTdemoView::OnSaveProjectImg)
	ON_UPDATE_COMMAND_UI(ID_SAVE_PROJECT, &CCTdemoView::OnUpdateSaveProjectImg)
	ON_COMMAND(ID_TOOLBAR_CONV, &CCTdemoView::OnToolbarConvolute)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_CONV, &CCTdemoView::OnUpdateToolbarConvolute)
	ON_COMMAND(ID_TOOLBAR_BKP, &CCTdemoView::OnToolbarBackProject)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_BKP, &CCTdemoView::OnUpdateToolbarBackProject)
	ON_COMMAND(ID_SAVE_BACK_PROJECT_IMG, &CCTdemoView::OnSaveBackProjectImg)
	ON_UPDATE_COMMAND_UI(ID_SAVE_BACK_PROJECT_IMG, &CCTdemoView::OnUpdateSaveBackProjectImg)
	ON_COMMAND(ID_SAVE_AFTER_FILTER_IMG, &CCTdemoView::OnSaveAfterFilterImg)
	ON_UPDATE_COMMAND_UI(ID_SAVE_AFTER_FILTER_IMG, &CCTdemoView::OnUpdateSaveAfterFilterImg)
	ON_COMMAND(ID_TOOLBAR_POPUP, &CCTdemoView::OnToolbarPopupImage)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_POPUP, &CCTdemoView::OnUpdateToolbarPopupImage)
	ON_COMMAND(ID_CONV_USING_GPU, &CCTdemoView::OnConvoluteUsingGpu)
	ON_UPDATE_COMMAND_UI(ID_CONV_USING_GPU, &CCTdemoView::OnUpdateConvoluteUsingGpu)
	ON_COMMAND(ID_BKP_USING_GPU, &CCTdemoView::OnBackProjectUsingGpu)
	ON_UPDATE_COMMAND_UI(ID_BKP_USING_GPU, &CCTdemoView::OnUpdateBackProjectUsingGpu)
	ON_COMMAND(ID_TOOLBAR_ZOOM_IN, &CCTdemoView::OnToolbarZoomIn)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_ZOOM_IN, &CCTdemoView::OnUpdateToolbarZoomIn)
	ON_COMMAND(ID_TOOLBAR_ZOOM_OUT, &CCTdemoView::OnToolbarZoomOut)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_ZOOM_OUT, &CCTdemoView::OnUpdateToolbarZoomOut)
	ON_COMMAND(ID_TOOLBAR_ZOOM_DEFAULT, &CCTdemoView::OnToolbarZoomDefault)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_ZOOM_DEFAULT, &CCTdemoView::OnUpdateToolbarZoomDefault)
	ON_COMMAND(ID_TOOLBAR_FLIP_H, &CCTdemoView::OnToolbarFlipH)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_FLIP_H, &CCTdemoView::OnUpdateToolbarFlipH)
	ON_COMMAND(ID_TOOLBAR_FLIP_V, &CCTdemoView::OnToolbarFlipV)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_FLIP_V, &CCTdemoView::OnUpdateToolbarFlipV)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_PAN_PROJECT, &CCTdemoView::OnPanProject)
	ON_UPDATE_COMMAND_UI(ID_PAN_PROJECT, &CCTdemoView::OnUpdatePanProject)
	ON_COMMAND(ID_PAN_RECONSTRUCT, &CCTdemoView::OnPanReconstruct)
	ON_UPDATE_COMMAND_UI(ID_PAN_RECONSTRUCT, &CCTdemoView::OnUpdatePanReconstruct)
	ON_COMMAND(ID_FAN_SCAN_SETTINGS, &CCTdemoView::OnFanScanSettings)
	ON_COMMAND(ID_AMP_PAN_PROJECT, &CCTdemoView::OnGpuPanProject)
	ON_UPDATE_COMMAND_UI(ID_AMP_PAN_PROJECT, &CCTdemoView::OnUpdateGpuPanProject)
	ON_COMMAND(ID_CUDA_PAN_PROJECT, &CCTdemoView::OnCudaPanProject)
	ON_UPDATE_COMMAND_UI(ID_CUDA_PAN_PROJECT, &CCTdemoView::OnUpdateCudaPanProject)
	ON_COMMAND(ID_CHANGE_IMAGE_SHOW, &CCTdemoView::OnChangeImageShowNext)
	ON_COMMAND(ID_CHANGE_IMAGE_SHOW_PREV, &CCTdemoView::OnChangeImageShowPrev)
	ON_COMMAND(ID_RECONSTRUCT_IMAGE_SIZE, &CCTdemoView::OnReconstructImageSize)
	ON_UPDATE_COMMAND_UI(ID_RECONSTRUCT_IMAGE_SIZE, &CCTdemoView::OnUpdateReconstructImageSize)
END_MESSAGE_MAP()

// CCTdemoView ����/����

CCTdemoView::CCTdemoView()
{
	// ����CPU�ĺ�����Ŀȷ���˲���
	int nKernels = omp_get_num_procs();
	switch (nKernels / 6)
	{
	case 0 :m_nCpuMaxImageSize = 256; break;
	case 1 :m_nCpuMaxImageSize = 512; break;
	case 2 :m_nCpuMaxImageSize = 768; break;
	case 3 :m_nCpuMaxImageSize = 1024;break;
	default:m_nCpuMaxImageSize = 1280;break;
	}
	m_nCurrent = 0;
	m_pCurrent = NULL;
	m_PaintRect = CRect();
	m_ClientRect = CRect();
	m_bRePaint = false;
	m_bUsingGpu = false;
	m_bMovingImage = false;
	m_bRightButtonDown = false;
	m_hBppEditorDll = NULL;
	m_fZoomRate = 1.f;
	m_nPointSize = 2;
}

CCTdemoView::~CCTdemoView()
{
	// ref��http://bbs.csdn.net/topics/120089700
	// ����names����DLL֮�������ڴ棬�������ݳ�ԱӦ������DLL�ڷ���ġ�
	// ��˲�����ǰ�ͷ�DLL�����޸�ͼ��λ���ʱ���� DLL ��new��һ��ͼ��
	CCTdemoDoc *pDoc = GetDocument();
	SAFE_DELETE(pDoc->m_pImage);
	if (m_hBppEditorDll)
		FreeLibrary(m_hBppEditorDll);
}

BOOL CCTdemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CScrollView::PreCreateWindow(cs);
}

// CCTdemoView ����

void CCTdemoView::OnDraw(CDC* pDC)
{
	CCTdemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (!CHECK_IMAGE_NULL(m_pCurrent))
	{
		m_pCurrent->Draw(pDC->GetSafeHdc(), m_PaintRect, Gdiplus::InterpolationModeBicubic);
	}
	if (CheckPointInRect(m_ptLeftButtonUp, m_PaintRect))
		PaintSinglePoint(pDC, m_ptLeftButtonUp, m_nPointSize);
	if (m_bRightButtonDown && CheckPointInRect(m_ptMouseMoving, m_PaintRect))
		PaintSelectedRect(pDC, m_ptRightButtonDown, m_ptMouseMoving);
}


// CCTdemoView ��ӡ


void CCTdemoView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCTdemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CCTdemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CCTdemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CCTdemoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{

}


// CCTdemoView ���

#ifdef _DEBUG
void CCTdemoView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CCTdemoView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CCTdemoDoc* CCTdemoView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCTdemoDoc)));
	return (CCTdemoDoc*)m_pDocument;
}
#endif //_DEBUG


// CCTdemoView ��Ϣ�������


BOOL CCTdemoView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CScrollView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CCTdemoView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���
	CCTdemoDoc* pDoc = GetDocument();
	int width = 0;
	int height = 0;
	if (!CHECK_IMAGE_NULL(pDoc->m_pImage))
	{
		width = pDoc->m_nWidth;
		height = pDoc->m_nHeight;
		m_nCurrent = 0;
	}
	else if (!CHECK_IMAGE_NULL(pDoc->m_pProject))
	{
		width = pDoc->m_nAnglesNum;
		height = pDoc->m_nRaysNum;
		m_nCurrent = 1;
	}
	// ���û�ͼ����
	SetPaintRect(0, 0, width, height);
}


// ������m_PaintRect�����ػ�һ��
void CCTdemoView::RePaint()
{
	if (m_bRePaint)
	{
		InvalidateRect(&m_PaintRect);
		m_bRePaint = false;
	}
}


// ���Ƶ�����
void CCTdemoView::PaintSinglePoint(CDC* pDC, CPoint &point, int nSize)
{
	if (m_bMovingImage) return;
	ShowRGBValue(point);
	CRect rect(point.x - nSize, point.y - nSize, point.x + nSize, point.y + nSize);
	pDC->Ellipse(&rect);
}


void CCTdemoView::PaintSelectedRect(CDC* pDC, CPoint &LeftTop, CPoint &RightBottom)
{
	pDC->MoveTo(LeftTop);
	pDC->LineTo(LeftTop.x, RightBottom.y);
	pDC->MoveTo(LeftTop.x, RightBottom.y);
	pDC->LineTo(RightBottom);
	pDC->MoveTo(RightBottom);
	pDC->LineTo(RightBottom.x, LeftTop.y);
	pDC->MoveTo(RightBottom.x, LeftTop.y);
	pDC->LineTo(LeftTop);
}


// ����Ƿ�������ţ����Ų��ᳬ�磬�����������ź�Ļ�ͼ����.
bool CCTdemoView::CheckZoomAble(float xRate, float yRate)
{
	int right = m_PaintRect.left + xRate * m_pCurrent->GetWidth();
	int bottom = m_PaintRect.top + xRate * m_pCurrent->GetHeight();
	if (right < 0 || bottom < 0)
		return false;
	m_PaintRect.right = right;
	m_PaintRect.bottom = bottom;
	return true;
}


// �Ŵ��ͼ������ͼ���СΪ����
bool CCTdemoView::ZoomPaintRect(float xRate, float yRate)
{
	if (!CheckZoomAble(xRate, yRate)) return false;
	if (m_PaintRect.right - m_PaintRect.left > 10240)
		m_PaintRect.right = m_PaintRect.left + 10240;
	if (m_PaintRect.bottom - m_PaintRect.top > 10240)
		m_PaintRect.bottom = m_PaintRect.top + 10240;
	SetScrollSizes(MM_TEXT, CSize(m_PaintRect.right, m_PaintRect.bottom));
	Invalidate(TRUE);
	return true;
}


// ���û�ͼ����
void CCTdemoView::SetPaintRect(int left, int top, int right, int bottom)
{
	// ���ɹ���ʾ��ͼ��
	if (right - left > 10240)
		right = left + 10240;
	if (bottom - top > 10240)
		bottom = top + 10240;
	m_PaintRect.left = left;
	m_PaintRect.top = top;
	m_PaintRect.right = right;
	m_PaintRect.bottom = bottom;
	SetScrollSizes(MM_TEXT, CSize(max(0, right), max(0, bottom)));
	Invalidate(TRUE);
}


// �ƶ���ͼ��������(dx, dy)
void CCTdemoView::MovePaintRect(int dx, int dy, CRect &rect)
{
	rect.left += dx;
	rect.right += dx;
	rect.top += dy;
	rect.bottom += dy;
}


// �ػ汳��
BOOL CCTdemoView::OnEraseBkgnd(CDC* pDC)
{
	// 1	2	3
	// 4	5	6
	// 7	8	9
	// ����5�����ͼ����
	pDC->PatBlt(0, 0, m_PaintRect.left, m_ClientRect.bottom, PATCOPY);	// 1 4 7
	pDC->PatBlt(0, 0, m_ClientRect.right, m_PaintRect.top, PATCOPY);	// 1 2 3
	pDC->PatBlt(m_PaintRect.right, 0, m_ClientRect.right - m_PaintRect.right, m_ClientRect.bottom, PATCOPY);	// 3 6 9	
	pDC->PatBlt(0, m_PaintRect.bottom, m_ClientRect.right, m_ClientRect.bottom - m_PaintRect.bottom, PATCOPY);	// 7 8 9

	return TRUE;
}

// �ĵ���С�ı�ʱ��Ӧ����
void CCTdemoView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	GetClientRect(&m_ClientRect);
}

// �ı�ͼ��λ���
void CCTdemoView::OnToolbarChangeBpp()
{
	// TODO: �ڴ���������������
	typedef void (*lpFun)(CyImage*);
	if (m_hBppEditorDll == NULL)
	{
		m_hBppEditorDll = LoadLibrary(_T("ImageBppEditor.dll"));
		if (m_hBppEditorDll == NULL)
		{
			MessageBox(_T("\"ImageBppEditor.dll\"����û�гɹ���"));
			return;
		}
	}
	//����ָ��
	lpFun pDllFunc = (lpFun)GetProcAddress(m_hBppEditorDll, "ImageBppEditor");
	if (pDllFunc == NULL)
	{
		MessageBox(_T("\"ImageBppEditor.dll\"����Ѱ��ʧ�ܣ�"));
		return;
	}
	CCTdemoDoc *pDoc = GetDocument();
	// ### ע��temp�Ŀռ�����DLL����ġ� ###
	pDllFunc(m_pCurrent);
	m_pCurrent->UpdateInfomation();
	if (m_pCurrent->m_nyBpp != pDoc->m_nBPP)
		pDoc->UpdateImageInfomation();
	Invalidate(TRUE);
}


void CCTdemoView::OnUpdateToolbarChangeBpp(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!CHECK_IMAGE_NULL(m_pCurrent));
}

// ��������Ϣ
BOOL CCTdemoView::PreTranslateMessage(MSG* pMsg)
{
	return CScrollView::PreTranslateMessage(pMsg);
}


// �����ݼ�
int CCTdemoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

// ��ת��ǰͼƬ
void CCTdemoView::OnToolbarTransform()
{
	CDlgRotateAngle dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		if (dlg.m_fRotateAngle == 0)
			return;
		int Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen;
		float* pSrc = m_pCurrent->Rotate(RAD(dlg.m_fRotateAngle), 0, 0, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen);
		m_pCurrent->Create(pSrc, NewWidth, NewHeight, NewRowlen);
		SAFE_DELETE(pSrc);
		Invalidate();
	}
}


void CCTdemoView::OnUpdateToolbarTransform(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!CHECK_IMAGE_NULL(m_pCurrent));
}

// ԭʼͼ��radon�任������ͶӰͼ��.
void CCTdemoView::OnToolbarProject()
{
	// TODO: �ڴ���������������
	CCTdemoDoc *pDoc = GetDocument();
	if (pDoc->m_nBPP > 8)
	{
		MessageBox(_T("ͼ�������ת��Ϊ8λɫͼ��"), _T("��ʾ"), MB_OK | MB_ICONINFORMATION);
		return;
	}
	if (pDoc->m_nWidth > m_nCpuMaxImageSize || pDoc->m_nHeight > m_nCpuMaxImageSize)
	{
		int Stat = MessageBox(_T("ͼ��ߴ�ϴ���CPU����ȽϷ�ʱ���Ƿ���٣�\n����CUDA���٣��������˵�->�༭->����ͶӰ��"), 
			_T("��ʾ"), MB_YESNOCANCEL | MB_ICONINFORMATION);
		if (Stat == IDNO)
			pDoc->RadonTransform();
		else if (Stat == IDYES)
			pDoc->OnProjectUsingGpu();
	}
	else pDoc->RadonTransform();
}


void CCTdemoView::OnUpdateToolbarProject(CCmdUI *pCmdUI)
{
	CCTdemoDoc* pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pImage));
}

// ͶӰ���ã��Ƕȸ�������������.
void CCTdemoView::OnProjectSettings()
{
	// TODO: �ڴ���������������
	CCTdemoDoc *pDoc = GetDocument();
	CDlgProjectSettings dlg(this);
	dlg.m_nRaysNum = pDoc->m_nRaysNum;
	dlg.m_nAnglesNum = pDoc->m_nAnglesNum;
	dlg.m_fAngleRange = pDoc->m_fAnglesRange;
	dlg.m_fSubPixel = pDoc->m_fRaysSeparation;
	if (dlg.DoModal() == IDOK)
	{
		pDoc->m_nRaysNum = dlg.m_nRaysNum;
		pDoc->m_nAnglesNum = dlg.m_nAnglesNum;
		if(dlg.m_fAngleRange > 0)
		{
			pDoc->m_fAnglesRange = dlg.m_fAngleRange;
			pDoc->m_fAnglesSeparation = pDoc->m_fAnglesRange / pDoc->m_nAnglesNum;
		}
		if (dlg.m_fSubPixel > 0)
			pDoc->m_fRaysSeparation = dlg.m_fSubPixel;
	}
}

// ����ͶӰ��ͼ��
void CCTdemoView::OnSaveProjectImg()
{
	CCTdemoDoc *pDoc = GetDocument();
	if (pDoc->m_pProject->Save() != true)
		MessageBox(_T("�������⣬����ͼ��ʧ�ܡ�"), _T("����"), MB_OK | MB_ICONWARNING);
}


void CCTdemoView::OnUpdateSaveProjectImg(CCmdUI *pCmdUI)
{
	CCTdemoDoc *pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pProject));
}

extern const char* cudaConvolute(float* h_pDst, float* h_prj, int row, int col, float delta_r, float w0);

void CCTdemoView::OnToolbarConvolute()
{
	// TODO: �ڴ���������������
	CCTdemoDoc *pDoc = GetDocument();
	if (m_bUsingGpu == false && (pDoc->m_nWidth > m_nCpuMaxImageSize || pDoc->m_nHeight > m_nCpuMaxImageSize))
	{
		if(MessageBox(_T("�����Ƚ��ܼ�����CPU����ȽϷ�ʱ���Ƿ���٣�\n����CUDA���٣��������˵�->�༭->���پ����"), 
			_T("��ʾ"), MB_YESNOCANCEL | MB_ICONINFORMATION) == IDYES)
			m_bUsingGpu = true;
	}
	CDlgConvluteW dlg;
	if (dlg.DoModal() == IDOK)
	{
		BeginWaitCursor();
		pDoc->m_pAfterFilter->Create(pDoc->m_nAnglesNum, pDoc->m_nRaysNum, 8);
		if (m_bUsingGpu)
		{
			const char* result = cudaConvolute(pDoc->m_pAfterFilter->m_pfFloat, pDoc->m_pProject->m_pfFloat, pDoc->m_nRaysNum, pDoc->m_nAnglesNum, 1.f, dlg.m_fW);
			if (result != NULL)
			{
				CString str(result);
				AfxMessageBox(_T("������ִ���CUDA ������Ϣ:\n") + str, MB_OK | MB_ICONWARNING);
				EndWaitCursor();
				m_bUsingGpu = false;
				return;
			}
			m_bUsingGpu = false;
		}
		else 
		{
			// �������������Ҫ��Ȩ
			switch(pDoc->m_nProjectionType)
			{
			case PROJECT_TYPE_PAR:
				Convolute(pDoc->m_pAfterFilter->m_pfFloat, pDoc->m_pProject->m_pfFloat, pDoc->m_nAnglesNum, pDoc->m_nRaysNum, 1.f, dlg.m_fW);
				break;
			case PROJECT_TYPE_PAN:
				Convolute(pDoc->m_pAfterFilter->m_pfFloat, pDoc->m_pProject->m_pfFloat, pDoc->m_nAnglesNum, pDoc->m_nRaysNum, 1.f, dlg.m_fW, pDoc->m_fPanSOR, pDoc->m_fPanSOD);
				break;
			default:
				break;
			}
		}
		// �������ݣ�����ʾͼ��
		pDoc->m_pAfterFilter->MemcpyFloatToByte();
		pDoc->OnWindowAfterFilter();
		EndWaitCursor();
	}
}


void CCTdemoView::OnUpdateToolbarConvolute(CCmdUI *pCmdUI)
{
	CCTdemoDoc *pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pProject));
}

extern const char* cudaBackProject(float* pDst, float* prj, int width, int height, int rays, int angles, float delta_r, float delta_fai);

// ��ͶӰ
void CCTdemoView::OnToolbarBackProject()
{
	// TODO: �ڴ���������������
	CCTdemoDoc *pDoc = GetDocument();
	pDoc->m_pReconstruct->Create(pDoc->m_nWidth, pDoc->m_nHeight, 8);
	BeginWaitCursor();
	float *pRadonSrc = NULL;
	if (!CHECK_IMAGE_NULL(pDoc->m_pAfterFilter))
		pRadonSrc = pDoc->m_pAfterFilter->m_pfFloat;
	else
		pRadonSrc = pDoc->m_pProject->m_pfFloat;
	if (m_bUsingGpu == false && (pDoc->m_nWidth > m_nCpuMaxImageSize || pDoc->m_nHeight > m_nCpuMaxImageSize))
	{
		if(MessageBox(_T("�����Ƚ��ܼ�����CPU����ȽϷ�ʱ���Ƿ���٣�\n����CUDA���٣��������˵�->�༭->�����ؽ���"), 
			_T("��ʾ"), MB_YESNOCANCEL | MB_ICONINFORMATION) == IDYES)
			m_bUsingGpu = true;
	}
	if (m_bUsingGpu)
	{
		const char* result = cudaBackProject(pDoc->m_pReconstruct->m_pfFloat, pRadonSrc, pDoc->m_nWidth, pDoc->m_nHeight, pDoc->m_nRaysNum, pDoc->m_nAnglesNum, 1.f, pDoc->m_fAnglesSeparation);
		if (result != NULL)
		{
			CString str(result);
			AfxMessageBox(_T("������ִ���CUDA ������Ϣ:\n") + str, MB_OK | MB_ICONWARNING);
			m_bUsingGpu = false;
			EndWaitCursor();
			return;
		}
		m_bUsingGpu = false;
	}
	else
	{
		// ��������ͶӰ��Ҫ��Ȩ
		switch(pDoc->m_nProjectionType)
		{
		case PROJECT_TYPE_PAR:
			BackProject(pDoc->m_pReconstruct->m_pfFloat, pRadonSrc, pDoc->m_nWidth, pDoc->m_nHeight, pDoc->m_nRaysNum, pDoc->m_nAnglesNum, 1.f, pDoc->m_fAnglesSeparation);
			break;
		case PROJECT_TYPE_PAN:
			BackProject(pDoc->m_pReconstruct->m_pfFloat, pRadonSrc, pDoc->m_nWidth, pDoc->m_nHeight, pDoc->m_nRaysNum, pDoc->m_nAnglesNum, 1.f, pDoc->m_fAnglesSeparation, pDoc->m_fPanSOR, pDoc->m_fPanSOD);
			break;
		default:
			break;
		}
	}
	if (CHECK_IMAGE_NULL(pDoc->m_pImage))
		pDoc->m_pReconstruct->MemcpyFloatToByte();
	else
		pDoc->m_pReconstruct->MemcpyFloatToByteBounded(0, 255);
	pDoc->OnWindowBackpro();
	EndWaitCursor();
}


void CCTdemoView::OnUpdateToolbarBackProject(CCmdUI *pCmdUI)
{
	CCTdemoDoc *pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pProject));
}


void CCTdemoView::OnSaveBackProjectImg()
{
	CCTdemoDoc *pDoc = GetDocument();
	if(pDoc->m_pReconstruct->Save() != true)
		MessageBox(_T("�������⣬����ͼ��ʧ�ܡ�"), _T("����"), MB_OK | MB_ICONWARNING);
}


void CCTdemoView::OnUpdateSaveBackProjectImg(CCmdUI *pCmdUI)
{
	CCTdemoDoc *pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pReconstruct));
}


void CCTdemoView::OnSaveAfterFilterImg()
{
	CCTdemoDoc *pDoc = GetDocument();
	if (pDoc->m_pAfterFilter->Save() != true)
		MessageBox(_T("�������⣬����ͼ��ʧ�ܡ�"), _T("����"), MB_OK | MB_ICONWARNING);
}


void CCTdemoView::OnUpdateSaveAfterFilterImg(CCmdUI *pCmdUI)
{
	CCTdemoDoc *pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pAfterFilter));
}


void CCTdemoView::OnToolbarPopupImage()
{
	CCTdemoDoc *pDoc = GetDocument();
	pDoc->Popup(m_pCurrent);
}


void CCTdemoView::OnUpdateToolbarPopupImage(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	CCTdemoDoc *pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(m_pCurrent));
}


// ������Щ��ʱ�ƻ�
void CCTdemoView::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case VIEWS_SYNCHRONIZE:
		if (m_bRePaint)
		{
			// do something
			m_bRePaint = false;
		}
		break;
	default:
		break;
	}
	CScrollView::OnTimer(nIDEvent);
}


void CCTdemoView::OnConvoluteUsingGpu()
{
	// TODO: �ڴ���������������
	m_bUsingGpu = true;
	OnToolbarConvolute();
}


void CCTdemoView::OnUpdateConvoluteUsingGpu(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	CCTdemoDoc *pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pProject));
}


void CCTdemoView::OnBackProjectUsingGpu()
{
	// TODO: �ڴ���������������
	m_bUsingGpu = true;
	OnToolbarBackProject();
}


void CCTdemoView::OnUpdateBackProjectUsingGpu(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	CCTdemoDoc *pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pProject));
}


void CCTdemoView::OnToolbarZoomIn()
{
	if (CHECK_IMAGE_NULL(m_pCurrent))
		return;
	m_fZoomRate *= 2.f;
	if (m_fZoomRate > 128.f)
		m_fZoomRate = 128.f;
	if (!ZoomPaintRect(m_fZoomRate, m_fZoomRate))
		m_fZoomRate /= 2;
}


void CCTdemoView::OnUpdateToolbarZoomIn(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!CHECK_IMAGE_NULL(m_pCurrent));
	pCmdUI->SetCheck(m_fZoomRate > 1.f);
}


void CCTdemoView::OnToolbarZoomOut()
{
	if (CHECK_IMAGE_NULL(m_pCurrent))
		return;
	m_fZoomRate /= 2.f;
	if (m_fZoomRate < 1 / 128.f)
		m_fZoomRate = 1 / 128.f;
	if (!ZoomPaintRect(m_fZoomRate, m_fZoomRate))
		m_fZoomRate *= 2;
}


void CCTdemoView::OnUpdateToolbarZoomOut(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!CHECK_IMAGE_NULL(m_pCurrent));
	pCmdUI->SetCheck(m_fZoomRate < 1.f);
}


void CCTdemoView::OnToolbarZoomDefault()
{
	if (CHECK_IMAGE_NULL(m_pCurrent))
		return;
	m_fZoomRate = 1.f;
	ZoomPaintRect(m_fZoomRate, m_fZoomRate);
}


void CCTdemoView::OnUpdateToolbarZoomDefault(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!CHECK_IMAGE_NULL(m_pCurrent));
	pCmdUI->SetCheck(m_fZoomRate == 1.f);
}


void CCTdemoView::OnToolbarFlipH()
{
	if (CHECK_IMAGE_NULL(m_pCurrent))
		return;
	int width, height, rowlen, bpp, channel;
	m_pCurrent->GetInfomation(width, height, rowlen, bpp, channel);
	BYTE* temp = new BYTE[height * rowlen];
	BYTE* head = m_pCurrent->GetHeadAddress();
	memcpy(temp, head, height * rowlen);

#pragma omp parallel for
	for (int r = 0; r < height; ++r)
	{
		for (int c = 0; c < width; ++c)
		{
			memcpy(head + c * channel + r * rowlen, temp + ( width - 1 - c ) * channel + r * rowlen, channel);
		}
	}
	SAFE_DELETE(temp);
	Invalidate(TRUE);
}


void CCTdemoView::OnUpdateToolbarFlipH(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!CHECK_IMAGE_NULL(m_pCurrent));
}


void CCTdemoView::OnToolbarFlipV()
{
	if (CHECK_IMAGE_NULL(m_pCurrent))
		return;
	int width, height, rowlen, bpp, channel;
	m_pCurrent->GetInfomation(width, height, rowlen, bpp, channel);
	BYTE* temp = new BYTE[height * rowlen];
	BYTE* head = m_pCurrent->GetHeadAddress();
	memcpy(temp, head, height * rowlen);

#pragma omp parallel for
	for (int r = 0; r < height; ++r)
	{
		memcpy(head + r * rowlen, temp + (height - 1 - r) * rowlen, rowlen);
	}
	SAFE_DELETE(temp);
	Invalidate(TRUE);
}


void CCTdemoView::OnUpdateToolbarFlipV(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!CHECK_IMAGE_NULL(m_pCurrent));
}


bool CCTdemoView::CheckPointInRect(CPoint &point, CRect &rect)
{
	if (rect.left < point.x && point.x < rect.right && rect.top < point.y && point.y < rect.bottom )
		return true;
	return false;
}


void CCTdemoView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_ptLeftButtonDown = point;
	m_ptMoveOrigin = point;
	m_bMovingImage = (!CHECK_IMAGE_NULL(m_pCurrent) && CheckPointInRect(m_ptMoveOrigin, m_PaintRect));
	CScrollView::OnLButtonDown(nFlags, point);
}


void CCTdemoView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_bMovingImage = false;
	m_ptLeftButtonUp = point;
	Invalidate(TRUE);
	CScrollView::OnLButtonUp(nFlags, point);
}


void CCTdemoView::ShowRGBValue(CPoint &point)
{
	CDC *pDC = GetDC();
	COLORREF ref = pDC->GetPixel(point);
	int R = GetRValue(ref);
	int G = GetGValue(ref);
	int B = GetBValue(ref);
	CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->GetMainWnd();
	CMFCStatusBar *pStatBar = pFrame->GetStatusBar();
	CString text;
	text.Format(_T("RGB = (%d, %d, %d)"), R, G, B);
	pStatBar->SetWindowTextW(text);
}


void CCTdemoView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_ptMouseMoving = point;
	if (m_bMovingImage)
	{
		int dx = m_ptMouseMoving.x - m_ptMoveOrigin.x;
		int dy = m_ptMouseMoving.y - m_ptMoveOrigin.y;
		MovePaintRect(dx, dy, m_PaintRect);
		Invalidate(TRUE);
		m_ptMoveOrigin = m_ptMouseMoving;
	}
	if (m_bRightButtonDown)
		Invalidate(TRUE);
	CScrollView::OnMouseMove(nFlags, point);
}


BOOL CCTdemoView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (zDelta > 0)
	{
		OnToolbarZoomIn();
	}
	else if (zDelta < 0)
	{
		OnToolbarZoomOut();
	}
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}


void CCTdemoView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_bRightButtonDown = true;
	m_ptRightButtonDown = point;
	
	CScrollView::OnRButtonDown(nFlags, point);
}


void CCTdemoView::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bRightButtonDown = false;
	m_ptRightButtonUp = point;
	m_SelectedRect = CRect(	min(m_ptRightButtonDown.x, m_ptRightButtonUp.x), 
							min(m_ptRightButtonDown.y, m_ptRightButtonUp.y), 
							max(m_ptRightButtonDown.x, m_ptRightButtonUp.x), 
							max(m_ptRightButtonDown.y, m_ptRightButtonUp.y));
	Invalidate(TRUE);
	ClientToScreen(&point);
	OnContextMenu(this, point);
}


void CCTdemoView::OnPanProject()
{
	CCTdemoDoc* pDoc = GetDocument();
	pDoc->Rand_Pan2(pDoc->m_fPanSOR, pDoc->m_fPanSOD, pDoc->m_nAnglesNum, pDoc->m_nRaysNum);
}


void CCTdemoView::OnUpdatePanProject(CCmdUI *pCmdUI)
{
	CCTdemoDoc* pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pImage));
}


void CCTdemoView::OnPanReconstruct()
{
	OnToolbarConvolute();
	OnToolbarBackProject();
}


void CCTdemoView::OnUpdatePanReconstruct(CCmdUI *pCmdUI)
{
	CCTdemoDoc* pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pProject));
}


void CCTdemoView::OnFanScanSettings()
{
	CCTdemoDoc *pDoc = GetDocument();
	pDoc->m_fRaysSeparation = 1.f;

	CDlgPanParameter dlg;
	dlg.m_nRaysNum = pDoc->m_nRaysNum;
	dlg.m_nAnglesNum = pDoc->m_nAnglesNum;
	dlg.m_fAngleRange = pDoc->m_fAnglesRange;
	dlg.m_fSubPixel = pDoc->m_fRaysSeparation;
	dlg.m_fPanSOR = pDoc->m_fPanSOR;
	dlg.m_fPanSOD = pDoc->m_fPanSOD;
	if (dlg.DoModal() == IDOK)
	{
		pDoc->m_nRaysNum = dlg.m_nRaysNum;
		pDoc->m_nAnglesNum = dlg.m_nAnglesNum;
		if(dlg.m_fAngleRange > 0)
		{
			pDoc->m_fAnglesRange = dlg.m_fAngleRange;
			pDoc->m_fAnglesSeparation = pDoc->m_fAnglesRange / pDoc->m_nAnglesNum;
			pDoc->m_fRaysSeparation = 1.f * pDoc->m_nImageDiag / pDoc->m_nRaysNum;
		}
		if (dlg.m_fSubPixel > 0)
			pDoc->m_fRaysSeparation = dlg.m_fSubPixel;
		if (dlg.m_fPanSOR > (pDoc->m_nImageDiag + 1) / 2)
			pDoc->m_fPanSOR = dlg.m_fPanSOR;
		else 
			AfxMessageBox(_T("SOR����Ч�����á�"));
		if (dlg.m_fPanSOD > pDoc->m_nImageDiag)
			pDoc->m_fPanSOD = dlg.m_fPanSOD;
		else 
			AfxMessageBox(_T("SOD����Ч������"));
	}
}


void CCTdemoView::OnGpuPanProject()
{
	CCTdemoDoc* pDoc = GetDocument();
	pDoc->m_bUsingAMP = true;
	pDoc->PanProject(pDoc->m_fPanSOR, pDoc->m_fPanSOD, pDoc->m_nAnglesNum, pDoc->m_nRaysNum);
}


void CCTdemoView::OnUpdateGpuPanProject(CCmdUI *pCmdUI)
{
	CCTdemoDoc* pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pImage));
}


void CCTdemoView::OnCudaPanProject()
{
	CCTdemoDoc* pDoc = GetDocument();
	pDoc->m_bUsingAMP = false;
	pDoc->PanProject(pDoc->m_fPanSOR, pDoc->m_fPanSOD, pDoc->m_nAnglesNum, pDoc->m_nRaysNum);
}


void CCTdemoView::OnUpdateCudaPanProject(CCmdUI *pCmdUI)
{
	CCTdemoDoc* pDoc = GetDocument();
	pCmdUI->Enable(!CHECK_IMAGE_NULL(pDoc->m_pImage));
}


void CCTdemoView::SetCurrentImage(CyImage* pImage)
{
	if (m_pCurrent == pImage)
		return;
	m_pCurrent = pImage;
	SetCurveWndImage(m_pCurrent);
	m_fZoomRate = 1.f;
	if (!CHECK_IMAGE_NULL(m_pCurrent))
		SetPaintRect(m_PaintRect.left, m_PaintRect.top, m_PaintRect.left + m_pCurrent->GetWidth(), m_PaintRect.top + m_pCurrent->GetHeight());
	else 
		SetPaintRect(m_PaintRect.left, m_PaintRect.top, m_PaintRect.left, m_PaintRect.top);
}



void CCTdemoView::SetCurveWndImage(CImage* pImage)
{
	CCTdemoDoc* pDoc = GetDocument();
	CCurveView* pView = (CCurveView*)pDoc->GetView(RUNTIME_CLASS(CCurveView));
	if (pView != NULL)
		pView->SetImage(pImage);
}


void CCTdemoView::OnChangeImageShowNext()
{
	CCTdemoDoc* pDoc = GetDocument();
	m_nCurrent++;
	switch (m_nCurrent)
	{
	case 0 : SetCurrentImage(pDoc->m_pImage);						break;
	case 1 : SetCurrentImage(pDoc->m_pProject);						break;
	case 2 : SetCurrentImage(pDoc->m_pAfterFilter);					break;
	case 3 : SetCurrentImage(pDoc->m_pReconstruct);m_nCurrent = -1;	break;
	default:
		break;
	}
}

void CCTdemoView::OnChangeImageShowPrev()
{
	CCTdemoDoc* pDoc = GetDocument();
	m_nCurrent--;
	if (m_nCurrent == -1)
		m_nCurrent = 3;
	switch (m_nCurrent)
	{
	case 0 : SetCurrentImage(pDoc->m_pImage);						break;
	case 1 : SetCurrentImage(pDoc->m_pProject);						break;
	case 2 : SetCurrentImage(pDoc->m_pAfterFilter);					break;
	case 3 : SetCurrentImage(pDoc->m_pReconstruct);					break;
	default:
		break;
	}
}


void CCTdemoView::OnReconstructImageSize()
{
	CCTdemoDoc* pDoc = GetDocument();
	pDoc->SetReconstructImageSize();
}


void CCTdemoView::OnUpdateReconstructImageSize(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
}
