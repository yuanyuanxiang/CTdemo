// ImageView.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageScrollView.h"
#include "DlgImageViewer.h"

// CImageScrollView

IMPLEMENT_DYNCREATE(CImageScrollView, CScrollView)

CImageScrollView::CImageScrollView()
{
	m_pImage = NULL;
	m_bMovingImage = false;
	m_bRightButtonDown = false;
	m_bPaintRect = false;
	m_pImageQuickViewer = NULL;
}

CImageScrollView::~CImageScrollView()
{
	SAFE_DELETE(m_pImageQuickViewer);
}

BEGIN_MESSAGE_MAP(CImageScrollView, CScrollView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CImageScrollView 绘图

void CImageScrollView::OnDraw(CDC* pDC)
{
	if (m_pImage != NULL)
		m_pImage->Draw(pDC->GetSafeHdc(), m_PaintRect, Gdiplus::InterpolationModeBilinear);
	if (CheckPointInRect(m_ptLeftButtonUp, m_PaintRect))
		PaintSinglePoint(pDC, m_ptLeftButtonUp, 2);
	if (m_bPaintRect && CheckPointInRect(m_ptMouseMoving, m_PaintRect))
		PaintSelectedRect(pDC, m_ptRightButtonDown, m_ptMouseMoving);
}


// CImageScrollView 诊断

#ifdef _DEBUG
void CImageScrollView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CImageScrollView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG

int CImageScrollView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_ToolTipCtrl.Create(this, TTS_ALWAYSTIP);       
	m_ToolTipCtrl.AddTool(this, _T(""));   
	m_ToolTipCtrl.SetTipBkColor(RGB(0, 222, 0));
	m_ToolTipCtrl.SetMaxTipWidth(250);
	m_ToolTipCtrl.SetDelayTime(250);
	EnableToolTips(TRUE);

	m_pImageQuickViewer = new CDlgQuickViewer;
	m_pImageQuickViewer->Create(IDD_DLG_QUICK_VIEWER, this);
	m_pImageQuickViewer->ShowWindow(SW_HIDE);

	return 0;
}

BOOL CImageScrollView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 1	2	3
	// 4	5	6
	// 7	8	9
	// 其中5代表绘图区域
	if (m_pImage == NULL)
		return CScrollView::OnEraseBkgnd(pDC);
	CRect ClientRect;
	GetClientRect(&ClientRect);
	pDC->SetWindowOrg(0, 0);
	OnPrepareDC(pDC);			//进行坐标原点的匹配
	pDC->DPtoLP(&ClientRect);	//将视图坐标转换为文档坐标
	pDC->PatBlt(0, 0, m_PaintRect.left, ClientRect.bottom, PATCOPY);	// 1 4 7
	pDC->PatBlt(0, 0, ClientRect.right, m_PaintRect.top, PATCOPY);		// 1 2 3
	pDC->PatBlt(m_PaintRect.right, 0, ClientRect.right - m_PaintRect.right, ClientRect.bottom, PATCOPY);	// 3 6 9	
	pDC->PatBlt(0, m_PaintRect.bottom, ClientRect.right, ClientRect.bottom - m_PaintRect.bottom, PATCOPY);	// 7 8 9

	return TRUE;
}


// 设置绘图区域
void CImageScrollView::SetPaintRect(int left, int top, int right, int bottom)
{
	// 最大可供显示的图像
	if (right - left > 10240)
		right = left + 10240;
	if (bottom - top > 10240)
		bottom = top + 10240;
	m_PaintRect.left = left;
	m_PaintRect.top = top;
	m_PaintRect.right = right;
	m_PaintRect.bottom = bottom;
	SetScrollSizes(MM_TEXT, CSize(right, bottom));
	Invalidate(TRUE);
}


void CImageScrollView::SetToolTipText(CString text)
{
	m_ToolTipCtrl.UpdateTipText(text, this);
}


void CImageScrollView::ShowRGBValue(CPoint &point)
{
	CDC *pDC = GetDC();
	COLORREF ref = pDC->GetPixel(point);
	int R = GetRValue(ref);
	int G = GetGValue(ref);
	int B = GetBValue(ref);
	CString text;
	text.Format(_T("RGB = (%d, %d, %d)"), R, G, B);
	SetToolTipText(text);
	ReleaseDC(pDC);
}


void CImageScrollView::PaintSinglePoint(CDC* pDC, CPoint &point, int nSize)
{
	if (m_bMovingImage) return;
	ShowRGBValue(point);
	CPoint pt = DP2LP(pDC, point);
	CRect rect(pt.x - nSize, pt.y - nSize, pt.x + nSize, pt.y + nSize);
	CPen *pOldPen, newPen;
	newPen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	pOldPen = pDC->SelectObject(&newPen);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->MoveTo(rect.left, rect.bottom);
	pDC->LineTo(rect.right, rect.bottom);
	pDC->MoveTo(rect.right, rect.bottom);
	pDC->LineTo(rect.right, rect.top);
	pDC->MoveTo(rect.right, rect.top);
	pDC->LineTo(rect.left, rect.top);
	pDC->SelectObject(pOldPen);
	newPen.DeleteObject();
}


void CImageScrollView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pImageQuickViewer->IsWindowVisible())
	{
		m_pImageQuickViewer->ShowWindow(SW_HIDE);
	}
	m_ptLeftButtonDown = point;
	m_ptMoveOrigin = point;
	m_bMovingImage = (!CheckImageNull() && CheckPointInRect(point, m_PaintRect));
	CScrollView::OnLButtonDown(nFlags, point);
}


// 检查是否可以缩放，缩放不会超界，并且设置缩放后的绘图区域
bool CImageScrollView::CheckZoomAble(float xRate, float yRate)
{
	int right = m_PaintRect.left + xRate * m_pImage->GetWidth();
	int bottom = m_PaintRect.top + xRate * m_pImage->GetHeight();
	if (right < 0 || bottom < 0)
		return false;
	m_PaintRect.right = right;
	m_PaintRect.bottom = bottom;
	return true;
}


// 放大绘图区，以图像大小为参照
bool CImageScrollView::ZoomPaintRect(float xRate, float yRate)
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


bool CImageScrollView::CheckImageNull()
{
	return (m_pImage == NULL);
}


bool CImageScrollView::CheckPointInRect(CPoint &point, CRect &rect)
{
	CPoint pt = DP2LP(point);
	if (rect.left < pt.x && pt.x < rect.right && rect.top < pt.y && pt.y < rect.bottom )
		return true;
	return false;
}


void CImageScrollView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bMovingImage = false;
	m_ptLeftButtonUp = point;
	if (CheckPointInRect(m_ptLeftButtonUp, m_PaintRect))
		Invalidate(TRUE);
	CScrollView::OnLButtonUp(nFlags, point);
}


LRESULT CImageScrollView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(message == WM_MOUSEMOVE && CheckPointInRect(m_ptMouseMoving, m_PaintRect))
	{
		MSG msg;
		msg.hwnd = m_hWnd;
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;

		m_ToolTipCtrl.RelayEvent(&msg);
	}
	return CScrollView::WindowProc(message, wParam, lParam);
}


void CImageScrollView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
}


void CImageScrollView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_ptMouseMoving = point;
	if (m_bMovingImage)
	{
		int dx = m_ptMouseMoving.x - m_ptMoveOrigin.x;
		int dy = m_ptMouseMoving.y - m_ptMoveOrigin.y;
		MovePaintRect(dx, dy, m_PaintRect);
		Invalidate(TRUE);
		m_ptMoveOrigin = m_ptMouseMoving;
	}

	if (m_bPaintRect)
	{
		m_ptLeftButtonUp.x = m_ptLeftButtonUp.y = -1;
		Invalidate(TRUE);
	}

	CScrollView::OnMouseMove(nFlags, point);
}


// 移动绘图区域，沿着(dx, dy)
void CImageScrollView::MovePaintRect(int dx, int dy, CRect &rect)
{
	rect.left += dx;
	rect.right += dx;
	rect.top += dy;
	rect.bottom += dy;
}



void CImageScrollView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_pImageQuickViewer->IsWindowVisible())
	{
		m_pImageQuickViewer->ShowWindow(SW_HIDE);
	}
	m_bRightButtonDown = true;
	m_ptRightButtonDown = point;
	m_bPaintRect = CheckPointInRect(m_ptRightButtonDown, m_PaintRect);
	CScrollView::OnRButtonDown(nFlags, point);
}


void CImageScrollView::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bRightButtonDown = false;
	m_ptRightButtonUp = point;
	if (m_ptRightButtonDown == m_ptRightButtonUp)
	{
		m_bPaintRect = false;
		return;
	}
	if (m_bPaintRect)
	{
		CDC* pDC = this->GetDC();
		m_SourceRect = CRect(min(m_ptRightButtonDown.x, m_ptRightButtonUp.x), min(m_ptRightButtonDown.y, m_ptRightButtonUp.y), 
			max(m_ptRightButtonDown.x, m_ptRightButtonUp.x), max(m_ptRightButtonDown.y, m_ptRightButtonUp.y));
		if (m_SourceRect.Width() < 4 || m_SourceRect.Height() < 4)
		{
			m_bPaintRect = false;
			ReleaseDC(pDC);
			return;
		}
		CPoint scree_point = m_ptRightButtonUp;
		ClientToScreen(&scree_point);
		m_pImageQuickViewer->MoveWindow(scree_point.x + 32, scree_point.y, QUICK_VIEWER_SIZE, QUICK_VIEWER_SIZE);
		// https://msdn.microsoft.com/zh-cn/library/71eseab0
		// 标识 CWnd 工作区的设备上下文，如果成功;否则，返回值是 NULL。
		// 指针可能是瞬态的，不应存储以供将来使用。
		m_pImageQuickViewer->SetParentCDC(pDC);
		ShowQuickViewerDlg(3000, SW_SHOWDEFAULT);

		m_bPaintRect = false;
	}
	CScrollView::OnRButtonUp(nFlags, point);
}


void CImageScrollView::PaintSelectedRect(CDC* pDC, CPoint &LeftTop, CPoint &RightBottom)
{
	CPoint lt = DP2LP(pDC, LeftTop);
	CPoint rb = DP2LP(pDC, RightBottom);
	pDC->MoveTo(lt);
	pDC->LineTo(lt.x, rb.y);
	pDC->MoveTo(lt.x, rb.y);
	pDC->LineTo(rb);
	pDC->MoveTo(rb);
	pDC->LineTo(rb.x, lt.y);
	pDC->MoveTo(rb.x, lt.y);
	pDC->LineTo(lt);
}

void CImageScrollView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == HIDE_WINDOW_QUICK_VIEWER)
	{
		this->Invalidate(TRUE);
		if (m_pImageQuickViewer->IsWindowVisible())
			m_pImageQuickViewer->ShowWindow(SW_HIDE);
		KillTimer(HIDE_WINDOW_QUICK_VIEWER);
	}

	CScrollView::OnTimer(nIDEvent);
}


void CImageScrollView::ShowQuickViewerDlg(int nElapse, int show_type)
{
	if (m_pImageQuickViewer->IsWindowVisible())
		return;
	m_pImageQuickViewer->ShowWindow(show_type);
	m_pImageQuickViewer->SetSourceRect(m_SourceRect);
	SetTimer(HIDE_WINDOW_QUICK_VIEWER, nElapse, NULL);
}


void CImageScrollView::CopySelectedImage()
{
	if (OpenClipboard())
	{
		EmptyClipboard();
		int width = m_SourceRect.Width() - 2;
		int height = m_SourceRect.Height() - 2;
		if (width < 4 || height < 4)
		{
			CloseClipboard();
			return;
		}

		ShowQuickViewerDlg(1000, SW_SHOW);

		CDC* pSrcDC = GetDC();
		CBitmap cBmp;
		cBmp.CreateCompatibleBitmap(pSrcDC, width, height);
		CDC MemDC;
		MemDC.CreateCompatibleDC(pSrcDC);
		CBitmap* pOldBitmap = MemDC.SelectObject(&cBmp);
		StretchBlt(MemDC, 0, 0, width, height, pSrcDC->GetSafeHdc(), m_SourceRect.left + 1, m_SourceRect.top + 1, width, height, SRCCOPY);
		SetClipboardData(CF_BITMAP, cBmp.GetSafeHandle());
		CloseClipboard();

		MemDC.SelectObject(pOldBitmap);
		MemDC.DeleteDC();
		ReleaseDC(pSrcDC);
		BITMAP bmp = {0};
		cBmp.GetBitmap(&bmp);
		cBmp.Detach();
	}
}


void CImageScrollView::SaveSelectedImage(CString path)
{
	int width = m_SourceRect.Width() - 2;
	int height = m_SourceRect.Height() - 2;
	if (width < 4 || height < 4)
		return;
	
	CDC* pSrcDC = GetDC();
	CBitmap cBmp;   
	cBmp.CreateCompatibleBitmap(pSrcDC, width, height);
	CDC MemDC;
	MemDC.CreateCompatibleDC(pSrcDC);
	CBitmap* pOldBitmap = MemDC.SelectObject(&cBmp);
	StretchBlt(MemDC, 0, 0, width, height, pSrcDC->GetSafeHdc(), m_SourceRect.left + 1, m_SourceRect.top + 1, width, height, SRCCOPY);
	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC();
	ReleaseDC(pSrcDC);

	BITMAP bmp = {0};
	cBmp.GetBitmap(&bmp);
	long Length = bmp.bmHeight * bmp.bmWidthBytes;
	BYTE* pSrc = new BYTE[Length];
	cBmp.GetBitmapBits(Length, pSrc);
	// 给目标地址拷贝数据
	CImage* pImage = new CImage;
	pImage->Create(bmp.bmWidth, bmp.bmHeight, bmp.bmBitsPixel);
	BYTE* pDst = (BYTE*)pImage->GetBits() + (pImage->GetPitch() * (pImage->GetHeight() - 1));
	memcpy(pDst, pSrc, Length);
		if (FAILED(pImage->Save(path)))
			MessageBox(_T("保存图像失败。"), _T("警告"), MB_OK | MB_ICONWARNING);
	SAFE_DELETE(pImage);
	SAFE_DELETE(pSrc);
	cBmp.Detach();
}


CPoint CImageScrollView::DP2LP(const CPoint &point)
{
	CPoint pt = point;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&pt);
	return pt;
}


// 参看：http://blog.csdn.net/kevin_samuel/article/details/8288617
CRect CImageScrollView::DP2LP(const CRect &rect)
{
	CRect rt = rect;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&rt);
	return rt;
}


CPoint CImageScrollView::DP2LP(CDC* pDC, const CPoint &point)
{
	CPoint pt = point;
	OnPrepareDC(pDC);
	pDC->DPtoLP(&pt);
	return pt;
}


CRect CImageScrollView::DP2LP(CDC* pDC, const CRect &rect)
{
	CRect rt = rect;
	OnPrepareDC(pDC);
	pDC->DPtoLP(&rt);
	return rt;
}