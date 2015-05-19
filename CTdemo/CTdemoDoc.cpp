
// CTdemoDoc.cpp : CCTdemoDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CTdemo.h"
#endif

#include "CTdemoDoc.h"
#include "CTdemoView.h"

#include <propkey.h>
#include <math.h>
#include <fstream>
using namespace std;
#include "ImageViewerDllAPIs.h"
#include <vector>
#include "IntSection.h"
#include <algorithm>
#include "DlgReconstructSettings.h"
#include "CurveView.h"
#include "MainFrm.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 不显示数据截断警告
#pragma warning(disable: 4244)

// CCTdemoDoc

IMPLEMENT_DYNCREATE(CCTdemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CCTdemoDoc, CDocument)
	ON_COMMAND(ID_WINDOW_PROJECT, &CCTdemoDoc::OnWindowProject)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_PROJECT, &CCTdemoDoc::OnUpdateWindowProject)
	ON_COMMAND(ID_PRJ_USING_GPU, &CCTdemoDoc::OnProjectUsingGpu)
	ON_COMMAND(ID_WINDOW_BACKPRO, &CCTdemoDoc::OnWindowBackpro)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_BACKPRO, &CCTdemoDoc::OnUpdateWindowBackpro)
	ON_COMMAND(ID_WINDOW_AFTER_FILTER, &CCTdemoDoc::OnWindowAfterFilter)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_AFTER_FILTER, &CCTdemoDoc::OnUpdateWindowAfterFilter)
END_MESSAGE_MAP()


// CCTdemoDoc 构造/析构

CCTdemoDoc::CCTdemoDoc()
{
	// 投影类型
	m_nProjectionType = PROJECT_TYPE_PAR;
	// 指针相关
	m_pImage = NULL;
	m_pProject = new CyImage;
	m_pAfterFilter = new CyImage;
	m_pReconstruct = new CyImage;
	m_pHead = NULL;
	// 图像信息
	m_ptOrigin = CPoint(0, 0);
	m_Ox = m_Oy = 0.f;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nRowlen = 0;
	m_nBPP = 0;
	m_nChannel = 0;
	m_nImageDiag = 0;
	// 采样信息
	m_fAnglesRange = PI;
	m_nAnglesNum = 180;
	m_nRaysNum = 360;
	m_fRaysSeparation = 0.5f;
	m_fAnglesSeparation = PI / 180;
	m_nDetectorCenter = 0;
	// 启用AMP
	m_bUsingAMP = false;
}

CCTdemoDoc::~CCTdemoDoc()
{
	SAFE_DELETE(m_pImage);
	SAFE_DELETE(m_pProject);
	SAFE_DELETE(m_pAfterFilter);
	SAFE_DELETE(m_pReconstruct);
}

BOOL CCTdemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}


CFrameWnd* CCTdemoDoc::CreateNewWindow( CDocTemplate* pTemplate,CDocument* pDocument)
{
	ASSERT_VALID(pTemplate); 
	ASSERT_VALID(pDocument);   
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, NULL);
	if(pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame.\n");
		return NULL;
	}
	CRect rect;
	GetClientRect(pFrame->m_hWnd, &rect);
	pFrame->MoveWindow(0, 0,rect.Width(), rect.Height());
	ASSERT_KINDOF(CFrameWnd, pFrame);
	pTemplate->InitialUpdateFrame(pFrame, pDocument);

	return pFrame;
}


// CCTdemoDoc 序列化

void CCTdemoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CCTdemoDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CCTdemoDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:  strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CCTdemoDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCTdemoDoc 诊断

#ifdef _DEBUG
void CCTdemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCTdemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCTdemoDoc 命令


// 打开文档
BOOL CCTdemoDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// 检查是否导入投影文件
	CCTdemoApp* pApp = (CCTdemoApp*)AfxGetApp();
	if (pApp->CheckProjectionFile())
		return OpenProjectionFile(lpszPathName);

	m_pImage = new CyImage;
	m_pImage->Load(lpszPathName);
	if (m_pImage->IsNull())
	{
		SAFE_DELETE(m_pImage);
		return FALSE;
	}
	UpdateImageInfomation();
	InitScanningParameters();
	m_strFilePath = lpszPathName;
	CString temp = GetFileNameFromPath(m_strFilePath);
	int num = temp.ReverseFind('.');
	m_strFileName = temp.Left(num);
	m_strFilePostfix = temp.Right(temp.GetLength() - num);
	SetPathName(m_strFilePath);
	SetModifiedFlag(FALSE);

	CCTdemoView* pView = GetMainView();
	pView->SetCurrentImage(m_pImage);

	return TRUE;
}


BOOL CCTdemoDoc::OpenProjectionFile(LPCTSTR lpszPathName)
{
	m_pProject->Load(lpszPathName);
	if (m_pProject->IsNull())
	{
		SAFE_DELETE(m_pProject);
		return FALSE;
	}
	if (m_pProject->m_nyBpp != 8)
	{
		CString file = lpszPathName;
		AfxMessageBox(file + _T("\n不是有效的投影文件。") , MB_OK | MB_ICONERROR);
		SAFE_DELETE(m_pProject);
		return FALSE;
	}
	m_strFilePath = lpszPathName;
	CString temp = GetFileNameFromPath(m_strFilePath);
	int num = temp.ReverseFind('.');
	m_strFileName = temp.Left(num);
	m_strFilePostfix = temp.Right(temp.GetLength() - num);
	SetPathName(m_strFilePath, FALSE);
	SetModifiedFlag(FALSE);

	m_nAnglesNum = m_pProject->GetWidth();
	m_nRaysNum = m_pProject->GetHeight();
	int rowlen = abs(m_pProject->GetPitch());

	SetReconstructImageSize();

	CCTdemoView* pView = GetMainView();
	pView->SetCurrentImage(m_pProject);

	return TRUE;
}


void CCTdemoDoc::SetReconstructImageSize()
{
	CDlgReconstructSettings dlg;
	if (dlg.DoModal() == IDOK)
	{
		float w0 = dlg.m_fConvoluteW;
		m_fAnglesSeparation = dlg.m_fAnglesRange / m_nAnglesNum;
		m_nWidth = dlg.m_nWidth;								//图像宽度
		m_nHeight = dlg.m_nHeight;								//图像高度
		m_ptOrigin.x = (m_nWidth + 1) / 2;
		m_ptOrigin.y = (m_nHeight + 1) / 2;
		m_Ox = m_nWidth / 2.f;
		m_Oy = m_nHeight / 2.f;
		m_fRaysSeparation = 1.f * ComputeRaysNum(m_nWidth, m_nHeight) / m_nRaysNum;	//射线间距
		m_nDetectorCenter = (m_nRaysNum + 1) / 2;									//探测器中心
	}
}


// 从path中获取文件名(含后缀)，并返回
CString CCTdemoDoc::GetFileNameFromPath(CString path)
{
	int last = path.ReverseFind('\\');
	return path.Right(path.GetLength() - last - 1);
}

BOOL CCTdemoDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(m_pImage != NULL)
	{
		// 8位图像需以BMP格式保存，否则以24位深度保存。
		if (m_nBPP == 8)
			m_pImage->Save(lpszPathName, Gdiplus::ImageFormatBMP);
		else
			m_pImage->Save(lpszPathName);
		SetModifiedFlag(FALSE);
	}
	return TRUE;
	// return CDocument::OnSaveDocument(lpszPathName);
}


void CCTdemoDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	// TODO: 在此添加专用代码和/或调用基类

	CDocument::SetPathName(lpszPathName, bAddToMRU);
}


void CCTdemoDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: 在此添加专用代码和/或调用基类

	CDocument::SetTitle(lpszTitle);
}


BOOL CCTdemoDoc::SaveModified()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDocument::SaveModified();
}


void CCTdemoDoc::OnCloseDocument()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDocument::OnCloseDocument();
}


void CCTdemoDoc::Popup(CyImage* pImage)
{
	PopImageViewerDlg(pImage, 0, 0, 0);
}


// 更新图像信息：首地址、宽、高、每行字节数、通道。
void CCTdemoDoc::UpdateImageInfomation()
{
	if(CHECK_IMAGE_NULL(m_pImage))
		return;
	m_pHead = m_pImage->GetHeadAddress();		//首地址
	m_nWidth = m_pImage->GetWidth();			//图像宽度
	m_nHeight = m_pImage->GetHeight();			//图像高度
	m_nRowlen = abs(m_pImage->GetPitch());		//每行字节数
	m_nBPP = m_pImage->GetBPP();				//每像素位数
	m_nChannel = m_nBPP / 8;					//图像通道数
	m_nImageDiag = m_pImage->GetDiagLength();	//图像对角线长
	m_ptOrigin.x = (m_nWidth + 1) / 2;
	m_ptOrigin.y = (m_nHeight + 1) / 2;
	m_Ox = m_nWidth / 2.f;
	m_Oy = m_nHeight / 2.f;
}


void CCTdemoDoc::InitScanningParameters()
{
	m_nRaysNum = ComputeRaysNum(m_nWidth, m_nHeight);		//射线条数
	m_fRaysSeparation = 0.5f;								//射线间距
	m_nDetectorCenter = (m_nRaysNum + 1) / 2;				//探测器中心
	m_fPanSOR = m_nImageDiag * 3;
	m_fPanSOD = 2 * m_fPanSOR;
}


CChildFrame* CCTdemoDoc::GetChildFrame()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CChildFrame* pChild = (CChildFrame*)pMainFrame->GetActiveFrame();
	return pChild;
}


// 从文档获取活动视图指针。
// 使用如下： CYourView* pView=(CYourView*)GetView(RUNTIME_CLASS(CYourView));
CView* CCTdemoDoc::GetView(CRuntimeClass* pClass)
{ 
	CView* pView = NULL;
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		pView = GetNextView(pos);
		if(pView->IsKindOf(pClass))
			return pView;
		else
			continue;
	}
	if(pView->IsKindOf(pClass))
		return pView; 
	else
	{
		TRACE("不能定位视图.\n");
		return NULL;
	}
}


CCTdemoView* CCTdemoDoc::GetMainView()
{
	return (CCTdemoView*) GetView(RUNTIME_CLASS(CCTdemoView));
}


// 沿着各个方向投影，即计算投影图像
void CCTdemoDoc::RadonTransform()
{
	m_nProjectionType = PROJECT_TYPE_PAR;
	m_pProject->Create(m_nAnglesNum, m_nRaysNum, 8);
	BeginWaitCursor();
	m_pImage->Radon(m_pProject->m_pfFloat, m_fAnglesSeparation, m_nAnglesNum, m_fRaysSeparation, m_nRaysNum);
	m_pProject->MemcpyFloatToByte();
	OnWindowProject();
	EndWaitCursor();
}


// 查看投影图像
void CCTdemoDoc::OnWindowProject()
{
	PopImageViewerDlg(m_pProject, m_nAnglesNum, m_nRaysNum, m_nAnglesNum);
}


void CCTdemoDoc::OnUpdateWindowProject(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_pProject != NULL);
}

extern const char* cudaRadon(float* h_pDst, int RaysNum, int AnglesNum, float rays_separation, float angle_separation, BYTE* h_pSrc, int Width, int Height, int Rowlen, float fPixelDistance);

// 使用CUDA加速投影
void CCTdemoDoc::OnProjectUsingGpu()
{
	// TODO: 在此添加命令处理程序代码
	if (m_nBPP == 0)
		return;
	if (m_nBPP > 8)
	{
		AfxMessageBox(_T("图像必须先转化为8位色图像。"), MB_OK | MB_ICONINFORMATION);
		return;
	}
	m_nProjectionType = PROJECT_TYPE_PAR;
	BeginWaitCursor();
	m_pProject->Create(m_nAnglesNum, m_nRaysNum, 8);
	const char *result = cudaRadon(m_pProject->m_pfFloat, m_nRaysNum, m_nAnglesNum, 1.f, m_fAnglesSeparation, m_pHead, m_nWidth, m_nHeight, m_nRowlen, m_fRaysSeparation);
	if (result != NULL)
	{
		CString str(result);
		AfxMessageBox(_T("程序出现错误。CUDA 错误信息:\n") + str, MB_OK | MB_ICONWARNING);
		EndWaitCursor();
		return;
	}
	m_pProject->MemcpyFloatToByte();
	EndWaitCursor();

	OnWindowProject();
}


CString CCTdemoDoc::GetExeFilePath()
{
	TCHAR exepath[MAX_PATH];
	CString strdir, tmpdir;
	memset(exepath, 0, MAX_PATH);
	GetModuleFileName(NULL, exepath, MAX_PATH);
	tmpdir = exepath;
	strdir = tmpdir.Left(tmpdir.ReverseFind('\\'));
	return strdir;
}

void CCTdemoDoc::OnWindowBackpro()
{
	PopImageViewerDlg(m_pReconstruct, 0, 0, 0);
}


void CCTdemoDoc::OnUpdateWindowBackpro(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_pReconstruct != NULL);
}


void CCTdemoDoc::OnWindowAfterFilter()
{
	PopImageViewerDlg(m_pAfterFilter, 0, 0, 0);
}


void CCTdemoDoc::OnUpdateWindowAfterFilter(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_pAfterFilter != NULL);
}


/*
 射线源、图像中心、探测器中心在同一水平线，即坐标轴x轴上面，默认D = 2R = 图像对角线某个倍数
 扇形束扫描，生成投影数据。R-SO,D-SO',angles-旋转角度,rays-每角度射线数.（等角度扫描，坐标原点为图像左下角顶点）
*/
void CCTdemoDoc::Rand_Pan1(float R, float D, int angles, int rays)
{
	m_nProjectionType = PROJECT_TYPE_PAN;
	m_pProject->Create(m_nAnglesNum, m_nRaysNum, 8);
	m_fRaysSeparation = 1.f;
	float Rate = 1 / m_fRaysSeparation;

	float theta_0 = acos(m_nImageDiag / 2.f / R);
	float theta_n = PI - theta_0;
	m_fPan_delta_fai = (theta_n - theta_0) / angles;

	BeginWaitCursor();
	float x0 = (m_nWidth + 1) / 2.f;
	float y0 = (m_nHeight + 1) / 2.f;

#pragma omp parallel for
	for (int i = 0; i < angles; ++i)
	{
		int Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen;
		float ang = i * m_fAnglesSeparation;
		float *pSrc = m_pImage->Rotate(ang, x0, x0, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen);
		for (int j = 0; j < rays; ++j)
		{
			float k = tan(theta_0 + j * m_fPan_delta_fai - PI / 2);
			float c = R * k + y0;
			m_pProject->m_pfFloat[i + j * angles] = LineIntegrate(pSrc, NewWidth, NewHeight, NewRowlen, m_nChannel, 0, Xmin, Ymin, Xmax, Ymax, k, c);
		}
		SAFE_DELETE(pSrc);
	}
	EndWaitCursor();

	PopImageViewerDlg(m_pProject->m_pfFloat, angles, rays, angles);
}


// 扇形束扫描，生成投影数据。R-SO,D-SO',angles-旋转角度,rays-每角度射线数.（等间距扫描）
void CCTdemoDoc::Rand_Pan2(float R, float D, int angles, int rays)
{
	m_nProjectionType = PROJECT_TYPE_PAN;
	m_pProject->Create(m_nAnglesNum, m_nRaysNum, 8);
	m_fRaysSeparation = 1.f;
	float Rate = 1 / m_fRaysSeparation;

	float theta_0 = acos(m_nImageDiag / 2.f / R);
	float theta_n = PI - theta_0;
	m_fPan_delta_fai = (theta_n - theta_0) / angles;
	m_fPan_u0 = D / tan(theta_0);
	m_fPan_delta_u = (2.f * m_fPan_u0) / rays;

	CString str;
	// 程序段开始前取得系统运行时间(ms)
	long t1 = GetTickCount();

	BeginWaitCursor();
	float x0 = (m_nWidth + 1) / 2.f;
	float y0 = (m_nHeight + 1) / 2.f;

#pragma omp parallel for
	for (int i = 0; i < angles; ++i)
	{
		int Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen;
		float ang = i * m_fAnglesSeparation;
		float *pSrc = m_pImage->Rotate(ang, x0, x0, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen);
		for (int j = 0; j < rays; ++j)
		{
			float u = -m_fPan_u0 + j * m_fPan_delta_u;
			float k = -u / D;
			float c = -R * k + y0;
			m_pProject->m_pfFloat[i + j * angles] = LineIntegrate(pSrc, NewWidth, NewHeight, NewRowlen, m_nChannel, 0, Xmin, Ymin, Xmax, Ymax, k, c);
		}
		SAFE_DELETE(pSrc);
	}
	EndWaitCursor();

	// 程序段结束后取得系统运行时间(ms)
	long t2 = GetTickCount();
	// 前后之差即程序运行时间  
	str.Format(_T("提示：程序耗时 %d ms."), t2 - t1);
	AfxMessageBox(str);

	m_pProject->MemcpyFloatToByte();
	
	PopImageViewerDlg(m_pProject->m_pfFloat, angles, rays, angles);
}

extern void cudaPanRadon(float* pSrc, int src_width, int src_height, float* pDst, int pan_angles, int pan_rays, float scan_range, float R, float D);
extern void AmpRadon(float* pSrc, int src_width, int src_height, float* pDst, int pan_angles, int pan_rays, float scan_range, float R, float D);

void CCTdemoDoc::PanProject(float R, float D, int angles, int rays)
{
	m_nProjectionType = PROJECT_TYPE_PAN;
	m_pProject->Create(m_nAnglesNum, m_nRaysNum, 8);
	float *pZoom = NULL;
	m_fRaysSeparation = 1.f;
	float Rate = 1 / m_fRaysSeparation;
	int Width = Rate * m_nWidth;
	int Height = Rate * m_nHeight;
	pZoom = m_pImage->Zoom(Width, Height);

	float theta_0 = acos(m_nImageDiag / 2.f / R);
	float theta_n = PI - theta_0;
	m_fPan_delta_fai = (theta_n - theta_0) / angles;
	m_fPan_u0 = D / tan(theta_0);
	m_fPan_delta_u = (2.f * m_fPan_u0) / rays;

	CString str;
	// 程序段开始前取得系统运行时间(ms)
	long t1 = GetTickCount();

	BeginWaitCursor();
	if (m_bUsingAMP)
	{
		AmpRadon(pZoom, Width, Height, m_pProject->m_pfFloat, angles, rays, m_fAnglesRange, R, D);
		m_bUsingAMP = false;
	}
	else
	{
		cudaPanRadon(pZoom, Width, Height, m_pProject->m_pfFloat, angles, rays, m_fAnglesRange, R, D);
	}
	EndWaitCursor();

	// 程序段结束后取得系统运行时间(ms)
	long t2 = GetTickCount();
	// 前后之差即程序运行时间  
	str.Format(_T("提示：程序耗时 %d ms."), t2 - t1);
	AfxMessageBox(str);

	SAFE_DELETE(pZoom);

	m_pProject->MemcpyFloatToByte();

	PopImageViewerDlg(m_pProject->m_pfFloat, angles, rays, angles);
}


void CCTdemoDoc::ReconstructImage(CString path)
{
	m_pProject->Load(path);
	if (m_pProject->IsNull())
		return;
	if (m_pProject->GetBPP() != 8)
		return;

	int angles = m_pProject->GetWidth();
	int rays = m_pProject->GetHeight();
	int rowlen = abs(m_pProject->GetPitch());

	BYTE* head = m_pProject->GetHeadAddress();

	int rec_width = 256;
	int rec_height = 256;

	CDlgReconstructSettings dlg;
	if (dlg.DoModal() == IDOK)
	{
		float delta_fai = dlg.m_fAnglesRange / angles;
		float w0 = dlg.m_fConvoluteW;
		m_pAfterFilter->Create(angles, rays, 8);
		m_pReconstruct->Create(rec_width, rec_height, 8);
		Convolute(m_pAfterFilter->m_pfFloat, m_pProject->m_pfFloat, angles, rays, m_fRaysSeparation, w0);
		BackProject(m_pReconstruct->m_pfFloat, m_pAfterFilter->m_pfFloat, rec_width, rec_height, angles, rays, m_fRaysSeparation, delta_fai);
	}
}