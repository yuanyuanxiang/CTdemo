
// CTdemoDoc.h : CCTdemoDoc 类的接口
//

#pragma once
#include "CyImage.h"

class CCTdemoView;
class CCurveView;
class CChildFrame;

#define PROJECT_TYPE_PAR	0		//平行束
#define PROJECT_TYPE_PAN	1		//扇形束

class CCTdemoDoc : public CDocument
{
protected: // 仅从序列化创建
	CCTdemoDoc();
	DECLARE_DYNCREATE(CCTdemoDoc)

// 特性
public:
	// 投影类型
	int					m_nProjectionType;	//投影类型
	// 文件信息
	CString				m_strFilePath;		//文件路径
	CString				m_strFileName;		//文件名称
	CString				m_strFilePostfix;	//文件后缀
	// 指针相关
	CyImage*			m_pImage;			//原始图像
	CyImage*			m_pProject;			//投影图像
	CyImage*			m_pAfterFilter;		//滤波后的投影
	CyImage*			m_pReconstruct;		//重建图像
	BYTE*				m_pHead;			//原图首地址
	// 图像信息
	CPoint				m_ptOrigin;			//图片中心
	float				m_Ox;				//图片中心x坐标
	float				m_Oy;				//图片中心y坐标
	int					m_nWidth;			//图像宽度
	int					m_nHeight;			//图像高度
	int					m_nRowlen;			//每行字节数
	int					m_nBPP;				//每像素位数
	int					m_nChannel;			//图像通道数
	int 				m_nImageDiag;		//图像对角线长
	// 采样信息
	float				m_fSubPixel;		//亚像素级
	float				m_fAnglesRange;		//采样的范围
	int					m_nAnglesNum;		//采样角度个数
	int					m_nRaysNum;			//每个角度射线数
	float				m_fRaysSeparation;	//射线间距
	float				m_fAnglesSeparation;//角度间距
	int					m_nDetectorCenter;	//探测器中心

// 操作
public:
	void				Popup(CyImage* pImage);								//弹出图像预览对话框
	void				UpdateImageInfomation();							//更新图像信息
	void				InitScanningParameters();							//初始化采样参数
	CString				GetFileNameFromPath(CString path);					//从路径取得文件名
	CView*				GetView(CRuntimeClass* pClass);						// 获取视图
	CCTdemoView*		GetMainView();										// 获取主视图
	CChildFrame*		GetChildFrame();									// 获取子框架
	CString				GetExeFilePath();									// 获取当前exe文件路径
	BOOL				OpenProjectionFile(LPCTSTR lpszPathName);			// 打开投影文件
	void				ReconstructImage(CString path);						// 对数据进行重建
	void				SetReconstructImageSize();							// 设置重建图像大小

	// *** 平行束相关 ***
	void				RadonTransform();									// 计算投影图像

	// *** 扇形束相关 ***
	bool				m_bUsingAMP;
	float				m_fPanSOR;
	float				m_fPanSOD;
	float				m_fPan_u0;
	float				m_fPan_delta_u;
	float				m_fPan_delta_fai;
	void				RandPanDiffAngles(float R, float D, int angles, int rays);
	void				RandPanDiffRays(float R, float D, int angles, int rays);
	void				PanProject(float R, float D, int angles, int rays);

private:
	CFrameWnd* CreateNewWindow(CDocTemplate* pTemplate,CDocument* pDocument);

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CCTdemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL SaveModified();
	virtual void OnCloseDocument();
	afx_msg void OnWindowProject();
	afx_msg void OnUpdateWindowProject(CCmdUI *pCmdUI);
	afx_msg void OnProjectUsingGpu();
	afx_msg void OnWindowBackpro();
	afx_msg void OnUpdateWindowBackpro(CCmdUI *pCmdUI);
	afx_msg void OnWindowAfterFilter();
	afx_msg void OnUpdateWindowAfterFilter(CCmdUI *pCmdUI);
};