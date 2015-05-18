#pragma once

// 
// CCurveWnd - 实现了类似于PhotoShop中“曲线”功能，在陈德峰师兄指导下完成。
// 2014年4月， 首都师范大学 袁沅祥。
// 2015年5月 更新，主要解决了8位色、32位色图像带来的各种BUG。
// 

#include "resource.h"
#include "CurveWnd.h"

#include <vector>
#include "CurveView.h"
using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }	//安全删除指针p
#endif

#ifndef CHECK_IMAGE_NULL
#define CHECK_IMAGE_NULL(p) ( ((p) == NULL) || ((p)->IsNull()) ) //检查图像是空
#endif

#define IDC_COMBOBOX_RGB				1001
#define IDB_CHOOSE_CURVE				1002
#define IDB_CHOOSE_LINE					1003
#define IDB_CurveWnd_RESET				1004
#define IDB_CurveWnd_INVERSE			1005
#define IDB_IMPORT_CURVE				1006
#define IDB_EXPORT_CURVE				1007
#define IDB_CURVE_OK					1008
#define IDB_CURVE_CANCEL				1009
#define IDE_CurveWnd_XPOSITION			1010
#define IDE_CurveWnd_YPOSITION			1011
#define IDS_CurveWnd_XLABEL				1012
#define IDS_CurveWnd_YLABEL				1013
#define IDB_GROUP_BOX					1014

#define LINE_PATTERN_SPLINE             0
#define LINE_PATTERN_LINEAR             1
#define CHANNEL_RGB                     0
#define CHANNEL_RED                     1
#define CHANNEL_GREEN                   2
#define CHANNEL_BLUE                    3

typedef						float   d_type;				  //运算采用数据类型

struct peg
{
	CPoint*                           m_pt;               //坐标点
	d_type                       m_dRatioX;               //相对Rect位置X
	d_type                       m_dRatioY;               //相对Rext位置Y
};

typedef CList<peg, peg>               PEGS;               //存放peg的数据结构
typedef vector<d_type>           TRANSFORM;               //存放灰度变换的向量,2015.5.16修改为d_type类型
typedef vector<d_type>              VECTOR;               //样条插值函数与追赶法的参数，用vector向量存储

class CCurveDlg;

class CCurveWnd : public CWnd
{
	DECLARE_DYNAMIC(CCurveWnd)

public:
	CCurveWnd();
	virtual ~CCurveWnd();

	CCurveDlg* GetDocument();							  /*获取主窗口*/
	CCTdemoView* GetView();								  /*获取视图指针*/
	CCTdemoDoc* GetMainDoc();							  /*获取文档指针*/

	void SetImage(CImage* pImage);						  //设置图像指针
	BOOL GetHistogram();                                  //取得直方图
	void InitPegs();									  //初始化pegs
	void UpdateImageInfo();								  //更新图像信息
	BOOL PegIsExist(CPoint& point);                       //看peg是否存在
	POSITION AddNewPeg(CPoint& point);                    //增加新的peg
	BOOL RePlacePeg(CPoint& point);                       //改变某个peg
	BOOL UserWillDrag(CPoint& point);                     //用户想增加一个peg
	void SplineFunc();                                    //样条插值函数
	void PieceWiseFunc();                                 //分段线性函数
	BOOL CursorIsInPaintRect(CPoint& point);              //光标在绘图区域之内
	CPoint TransPositionToDialog(CPoint& point);          //转换到对话框默认的坐标系(左上角是零点)
	CPoint TransPositionToRectangle(CPoint& point);       //转换到以Rect左下角为原点的坐标系
	BOOL GrayTransform();                                 //根据曲线作灰度变换
	void Threshold();                                     //对超出[0,255]的灰度进行处理
	d_type* PreTransform();                               //对变换值预操作
	void InverseTransform();                              //图像反转
	void ImshowImmediately();                             //立即刷新图像
	d_type GetCurrentGrayTransform(d_type gray);          //获取当前灰度值的变换值
	void ShowGrayTransform();                             //插值并显示灰度变换曲线

	void ReShapeButtons(int nItemID, int nButtonPosX1, int nButtonPosX2, d_type Space, int Number);
	void ReshapePegs(bool bAllRefresh = false);			  //刷新pegs, true为刷新全部, 否则刷新当前
	void ReshapeTransform(bool bAllRefresh = false);	  //刷新通道变换, true为刷新全部, 否则刷新当前

	/*** 2015年5月 添加 ***/
	bool m_bModified;
	int m_nNewRowlen;
	int m_nNewChannel;
	int m_nNewlenData;
	void ApplyToImage();								  //应用到图像
	void MallocData();									  //分配缓存数据
	bool DetectModified();								  //检查图像是否更新
	bool DetectImageChanged();							  //检查图像是否变换

public:
	//按钮
	CComboBox*                         m_pComboBoxRGB;    //RGB通道:4
	CButton*                            m_pBnGroupBox;    //Group Box
	CMFCButton*                      m_pBnChooseCurve;    //曲线模式
	CMFCButton*                       m_pBnChooseLine;    //直线模式
	CMFCButton*                          m_pBnInverse;    //反转
	CMFCButton*                            m_pBnReset;    //重置
	CMFCButton*                      m_pBnImportCurve;    //导入曲线
	CMFCButton*                      m_pBnExportCurve;    //导出曲线
	CMFCButton*                               m_pBnOK;    //确定
	CMFCButton*                           m_pBnCancel;    //取消
	CEdit*                               m_pXPosition;    //X坐标
	CEdit*                               m_pYPosition;    //Y坐标
	CStatic*                                m_pXlabel;    //X标签
	CStatic*                                m_pYlabel;    //Y标签
	CImage*                                  m_pImage;    //CImage指针
	CFont*                                 m_pAppFont;    //字体
	int                                   m_nFontSize;    //字体大小
	CString                                m_FontName;    //字体名
	d_type*                          m_pfHistogram[4];    //直方图:4*256
	TRANSFORM                        m_V4Transform[4];    //灰度变换:4*m_nWidth
	PEGS                               m_pPegsList[4];    //图像选的点:4*m_nWidth
	CRect*                             m_pOnPaintRect;    //绘图区域
	CRect*                                 m_pWndSize;    //当前Wnd大小
	int                            m_nChannelSelected;    //当前通道
	BOOL                              m_bUserWillDrag;    //新增peg
	int                                  m_nRectWidth;    //绘图区宽度
	int                                 m_nRectHeight;    //绘图区高度
	d_type                              m_fWidthRatio;    //宽度缩放比
	d_type                             m_fHeightRatio;    //高度缩放比
	POSITION                          m_posCurrentPeg;    //当前拽动的peg
	int                                m_nLinePattern;    //线型
	BOOL                              m_bInverseImage;    //反转图像

	// 图像信息 - 图像可能会变化，因此图像信息不合适作为类成员变量.
	BYTE*                                     m_pBits;    //图像指针
	BYTE*                                   m_dataSrc;    //图像源数据
	BYTE*                                   m_curData;    //当前图像数据
	long                                m_nImageWidth;    //图像宽度
	long                               m_nImageHeight;    //图像高度
	long                             m_nPixelQuantity;    //像素个数
	long                                m_nbytesWidth;    //图像位宽
	int                                    m_nChannel;    //图像通道
	DWORD                                  m_nlenData;    //图像长度

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnChooseCurve();
	afx_msg void OnChooseLine();
	afx_msg void OnComboboxRgb();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCmdCurveWndReset();
	afx_msg void OnCmdCurveWndInverse();
	afx_msg void OnCmdImportCurve();
	afx_msg void OnCmdExportCurve();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCmdCurveOk();
	afx_msg void OnCmdCurveCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private://新增的私有函数
	VECTOR Spline(VECTOR& Node, VECTOR& Value, VECTOR& x0, d_type c1, d_type c2, int CASE = 2);
	d_type* Spline(d_type* Node, d_type* Value, int Node_Num, d_type* x0, int x0_len, d_type c1, d_type c2, int CASE = 2);
	void Chase(VECTOR& a, VECTOR& b, VECTOR& c, VECTOR& f, VECTOR& x, int n);
	void Chase(d_type* a, d_type* b, d_type* c, d_type* f, d_type* x, int n);
	COLORREF ColorSetting(int Channel, int Strength, int WhereUse = 0);
};