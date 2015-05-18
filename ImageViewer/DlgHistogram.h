#pragma once
#include "Resource.h"


// CDlgHistogram 对话框

class CDlgHistogram : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHistogram)

protected:
	// 新增成员
	double              m_pfHistogram[4][256];      //直方图:4*256
	double              m_pfTotalHist[4][256];      //累计直方图
	int                 m_V4Transform[4][256];      //灰度变换:4*256
	CRect               m_ClientRect;               //客户区
	CRect				m_PaintRect;				//绘图区
	int                 m_nWidth;                   //绘图区宽度
	int                 m_nHeight;                  //绘图区高度
	double              m_fWidthRatio;              //宽度伸缩比
	double              m_fHeightRatio;             //高度伸缩比
	bool                m_bInitialStatus;           //直方图是否成功
	int                 m_nChannelSelected;         //当前选择的通道

public:
	// new members
	void SetImage(CImage* pImage);					//设置图像指针
	bool GetHistogram();                            //获得直方图数据
	void ColorSetting(COLORREF &ref, int Channel, int Strength, int WhereUse = 0);

private:
	HACCEL				m_hAccel;	//快捷键
	CImage*				m_pImage;	//图像指针

public:
	CDlgHistogram(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgHistogram();

	// 对话框数据
	enum { IDD = IDD_DLG_HISTOGRAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChannelRgb();
	afx_msg void OnChannelRed();
	afx_msg void OnChannelGreen();
	afx_msg void OnChannelBlue();
	afx_msg void OnUpdateChannelRgb(CCmdUI *pCmdUI);
	afx_msg void OnUpdateChannelRed(CCmdUI *pCmdUI);
	afx_msg void OnUpdateChannelGreen(CCmdUI *pCmdUI);
	afx_msg void OnUpdateChannelBlue(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHkHistogramDlgRgb();
	afx_msg void OnHkHistogramDlgRed();
	afx_msg void OnHkHistogramDlgGreen();
	afx_msg void OnHkHistogramDlgBlue();
	virtual BOOL ContinueModal();
};