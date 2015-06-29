
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
#include "ImageViewerDllFuncs.h"
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
	m_pProject = NULL;
	m_pAfterFilter = NULL;
	m_pReconstruct = NULL;
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
	m_fSubPixel = 0.5f;
	m_fAnglesRange = PI;
	m_nAnglesNum = 180;
	m_nRaysNum = 360;
	m_fRaysDensity = 1.0f;
	m_fAnglesSeparation = PI / 180;
	m_nDetectorCenter = 0;
	// SO,SO'
	m_fPanSOR = 1.f;
	m_fPanSOD = 1.f;
	m_fPan_u0 = 1.f;
	m_fPan_delta_u = 1.f;
	m_fPan_delta_fai = 0.f;
	// 导数图像
	m_pfDBPImage = NULL;
	m_nCurrentFile = -1;
	m_nTotalFile = 0;
}

CCTdemoDoc::~CCTdemoDoc()
{
	SAFE_DELETE(m_pImage);
	SAFE_DELETE(m_pProject);
	SAFE_DELETE(m_pAfterFilter);
	SAFE_DELETE(m_pReconstruct);
	SAFE_DELETE(m_pfDBPImage);
}

BOOL CCTdemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	m_pImage = new CyImage;
	m_pProject = new CyImage;
	m_pAfterFilter = new CyImage;
	m_pReconstruct = new CyImage;

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

	// 如果使用OpenGL
	CCTdemoApp* pApp = (CCTdemoApp* )AfxGetApp();
	bool UsingOpenGL = pApp->m_bUsingOpenGL;
	if (UsingOpenGL)
		return TRUE;

	// 读取图像
	m_pImage = new CyImage;
	m_pImage->Load(lpszPathName);
	if (m_pImage->IsNull())
	{
		CString strFilePath = lpszPathName;
		AfxMessageBox(_T("读取文件\"") + strFilePath + _T("\"失败！"), MB_OK | MB_ICONERROR);
		SAFE_DELETE(m_pImage);
		return FALSE;
	}

	// 初始化图像指针
	m_pProject = new CyImage;
	m_pAfterFilter = new CyImage;
	m_pReconstruct = new CyImage;

	UpdateImageInfomation();
	InitScanningParameters();
	SetPathName(lpszPathName);
	SetModifiedFlag(FALSE);

	CCTdemoView* pView = GetMainView();
	pView->SetCurrentImage(m_pImage);

	InitFileList(lpszPathName);

	return TRUE;
}


void CCTdemoDoc::InitFileList(LPCTSTR lpszPathName)
{
	if (lpszPathName == NULL)
		return;
	m_strFilePath = lpszPathName;
	CString temp = GetFileNameFromPath(m_strFilePath);
	int num = temp.ReverseFind('.');
	m_strFileName = temp.Left(num);
	m_strFilePostfix = temp.Right(temp.GetLength() - num);
	// 获取文件所在文件夹
	int path_len = m_strFilePath.GetLength();
	int file_len = temp.GetLength();
	if (path_len - file_len >= 1)
		m_strFolder = m_strFilePath.Left(path_len - file_len - 1);
	m_vStrAllFiles = ScanDiskFile(m_strFolder);
	m_nCurrentFile = FindCurrentFileId(m_vStrAllFiles, m_strFileName + m_strFilePostfix);
	return;
}


int CCTdemoDoc::FindCurrentFileId(vector<CString>& vStrAllFiles, CString strFileName)
{
	m_nTotalFile = vStrAllFiles.size();
	for (int i = 0; i < m_nTotalFile; ++i)
	{
		if (vStrAllFiles[i] == strFileName)
			return i;
	}
	return -1;
}


void CCTdemoDoc::SetNextImage()
{
	// 检查当前文件是否更改并保存
	if (this->IsModified())
	{
		switch (AfxMessageBox(_T("图像已更改，是否保存？"), MB_ICONQUESTION | MB_YESNOCANCEL))
		{
		case IDYES:OnSaveDocument(m_strFilePath);	break;
		case IDNO:									break;
		case IDCANCEL:return;						break;
		default:break;
		}
	}

	m_nCurrentFile++;
	if (m_nCurrentFile == m_nTotalFile)
		m_nCurrentFile = 0;

	m_strFilePath = m_strFolder + _T("\\") + m_vStrAllFiles[m_nCurrentFile];
	CString temp = GetFileNameFromPath(m_strFilePath);
	int num = temp.ReverseFind('.');
	m_strFileName = temp.Left(num);
	m_strFilePostfix = temp.Right(temp.GetLength() - num);

	m_pImage->Load(m_strFolder + _T("\\") + m_vStrAllFiles[m_nCurrentFile]);
	SetTitle(m_strFileName);
	SetModifiedFlag(FALSE);
	UpdateImageInfomation();
	InitScanningParameters();
	CCTdemoView* pView = GetMainView();
	pView->SetCurrentImage(m_pImage, TRUE);
	pView->Invalidate(TRUE);
}


void CCTdemoDoc::SetPrevImage()
{
	// 检查当前文件是否更改并保存
	if (this->IsModified())
	{
		switch (AfxMessageBox(_T("图像已更改，是否保存？"), MB_ICONQUESTION | MB_YESNOCANCEL))
		{
		case IDYES:OnSaveDocument(m_strFilePath);	break;
		case IDNO:									break;
		case IDCANCEL:return;						break;
		default:break;
		}
	}

	m_nCurrentFile--;
	if (m_nCurrentFile == -1)
		m_nCurrentFile = m_nTotalFile - 1;

	m_strFilePath = m_strFolder + _T("\\") + m_vStrAllFiles[m_nCurrentFile];
	CString temp = GetFileNameFromPath(m_strFilePath);
	int num = temp.ReverseFind('.');
	m_strFileName = temp.Left(num);
	m_strFilePostfix = temp.Right(temp.GetLength() - num);

	m_pImage->Load(m_strFilePath);
	SetTitle(m_strFileName);
	SetModifiedFlag(FALSE);
	UpdateImageInfomation();
	InitScanningParameters();
	CCTdemoView* pView = GetMainView();
	pView->SetCurrentImage(m_pImage, TRUE);
	pView->Invalidate(TRUE);
}


void CCTdemoDoc::SetReconstructImageSize()
{
	CDlgReconstructSettings dlg;
	if (CHECK_IMAGE_NULL(m_pImage))
	{
		dlg.m_nWidth = dlg.m_nHeight = 256;
	}
	else
	{
		dlg.m_nWidth = m_pImage->GetWidth();
		dlg.m_nHeight = m_pImage->GetHeight();
	}
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
		m_fRaysDensity = 1.f * m_nRaysNum / ComputeRaysNum(m_nWidth, m_nHeight);	//射线密度
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


void CCTdemoDoc::Popup(float* pSrc, int Width, int Height, int Rowlen)
{
	PopImageViewerDlg(pSrc, Width, Height, Rowlen);
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
	m_fRaysDensity = 1.0f;									//射线密度
	m_nDetectorCenter = (m_nRaysNum + 1) / 2;				//探测器中心
	m_fPanSOR = m_nImageDiag * 8;
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
	float *pDst = m_pImage->Radon(m_fAnglesSeparation, m_nAnglesNum, m_fSubPixel, m_nRaysNum);
	EndWaitCursor();
	m_pProject->SetFloatData(pDst, m_nAnglesNum, m_nRaysNum);
	m_pProject->MemcpyFloatToByte();
	SAFE_DELETE(pDst);
	OnWindowProject();
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

extern const char* cudaRadon(float* h_pDst, int RaysNum, int AnglesNum, float pixel_separation, float angle_separation, BYTE* h_pSrc, int Width, int Height, int Rowlen);

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
	const char *result = cudaRadon(m_pProject->m_pfFloat, m_nRaysNum, m_nAnglesNum, m_fSubPixel, m_fAnglesSeparation, m_pHead, m_nWidth, m_nHeight, m_nRowlen);
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
void CCTdemoDoc::RandPanDiffAngles(float R, float D, int angles, int rays)
{
	m_nProjectionType = PROJECT_TYPE_PAN;
	m_pProject->Create(m_nAnglesNum, m_nRaysNum, 8);
	m_fRaysDensity = 1.f;

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
		float theta = i * m_fAnglesSeparation;
		float *pSrc = m_pImage->Rotate(theta, x0, y0, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen);
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
void CCTdemoDoc::RandPanDiffRays(float R, float D, int angles, int rays)
{
	m_nProjectionType = PROJECT_TYPE_PAN;
	m_pProject->Create(m_nAnglesNum, m_nRaysNum, 8);
	m_fRaysDensity = 1.f;

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
		float theta = i * m_fAnglesSeparation;
		float *pSrc = m_pImage->Rotate(theta, x0, y0, Xmin, Ymin, Xmax, Ymax, NewWidth, NewHeight, NewRowlen);
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

extern const char* cudaPanRadon(float* pSrc, int src_width, int src_height, float* pDst, int pan_angles, int pan_rays, float scan_range, float R, float D);

void CCTdemoDoc::PanProject(float R, float D, int angles, int rays)
{
	m_nProjectionType = PROJECT_TYPE_PAN;
	m_pProject->Create(m_nAnglesNum, m_nRaysNum, 8);
	float *pZoom = NULL;
	m_fRaysDensity = 1.f;
	float Rate = 1 / m_fRaysDensity;
	int Width = Rate * m_nWidth;
	int Height = Rate * m_nHeight;
	pZoom = m_pImage->Zoom(Width, Height, 0);

	float theta_0 = acos(m_nImageDiag / 2.f / R);
	float theta_n = PI - theta_0;
	m_fPan_delta_fai = (theta_n - theta_0) / angles;
	m_fPan_u0 = D / tan(theta_0);
	m_fPan_delta_u = (2.f * m_fPan_u0) / rays;

	CString str;
	// 程序段开始前取得系统运行时间(ms)
	long t1 = GetTickCount();

	BeginWaitCursor();
	const char* result = cudaPanRadon(pZoom, Width, Height, m_pProject->m_pfFloat, angles, rays, m_fAnglesRange, R, D);
	EndWaitCursor();

	// 程序段结束后取得系统运行时间(ms)
	long t2 = GetTickCount();

	if (result != NULL)
	{
		CString str(result);
		AfxMessageBox(_T("程序出现错误。CUDA 错误信息:\n") + str, MB_OK | MB_ICONWARNING);
		t2 = t1;
	}

	// 前后之差即程序运行时间  
	str.Format(_T("提示：程序耗时 %d ms."), t2 - t1);
	AfxMessageBox(str);

	SAFE_DELETE(pZoom);

	m_pProject->MemcpyFloatToByte();

	PopImageViewerDlg(m_pProject->m_pfFloat, angles, rays, angles);
}


// 新建空白文档
CCTdemoDoc* CCTdemoDoc::CreateNewDocument(LPCTSTR lpszPathName)
{
	CCTdemoDoc* pDoc = (CCTdemoDoc *)GetDocTemplate()->OpenDocumentFile(lpszPathName);
	if (lpszPathName != NULL)
		return pDoc;
	pDoc->m_pProject = new CyImage;
	pDoc->m_pAfterFilter = new CyImage;
	pDoc->m_pReconstruct = new CyImage;
	pDoc->m_pImage = new CyImage;
	return pDoc;
}