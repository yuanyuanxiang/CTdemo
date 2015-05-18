#pragma once
#include "Resource.h"


// CDlgHistogram �Ի���

class CDlgHistogram : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHistogram)

protected:
	// ������Ա
	double              m_pfHistogram[4][256];      //ֱ��ͼ:4*256
	double              m_pfTotalHist[4][256];      //�ۼ�ֱ��ͼ
	int                 m_V4Transform[4][256];      //�Ҷȱ任:4*256
	CRect               m_ClientRect;               //�ͻ���
	CRect				m_PaintRect;				//��ͼ��
	int                 m_nWidth;                   //��ͼ�����
	int                 m_nHeight;                  //��ͼ���߶�
	double              m_fWidthRatio;              //���������
	double              m_fHeightRatio;             //�߶�������
	bool                m_bInitialStatus;           //ֱ��ͼ�Ƿ�ɹ�
	int                 m_nChannelSelected;         //��ǰѡ���ͨ��

public:
	// new members
	void SetImage(CImage* pImage);					//����ͼ��ָ��
	bool GetHistogram();                            //���ֱ��ͼ����
	void ColorSetting(COLORREF &ref, int Channel, int Strength, int WhereUse = 0);

private:
	HACCEL				m_hAccel;	//��ݼ�
	CImage*				m_pImage;	//ͼ��ָ��

public:
	CDlgHistogram(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgHistogram();

	// �Ի�������
	enum { IDD = IDD_DLG_HISTOGRAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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