#pragma once

// 
// CCurveWnd - ʵ����������PhotoShop�С����ߡ����ܣ��ڳµ·�ʦ��ָ������ɡ�
// 2014��4�£� �׶�ʦ����ѧ Ԭ���顣
// 2015��5�� ���£���Ҫ�����8λɫ��32λɫͼ������ĸ���BUG��
// 

#include "resource.h"
#include "CurveWnd.h"

#include <vector>
#include "CurveView.h"
using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }	//��ȫɾ��ָ��p
#endif

#ifndef CHECK_IMAGE_NULL
#define CHECK_IMAGE_NULL(p) ( ((p) == NULL) || ((p)->IsNull()) ) //���ͼ���ǿ�
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

typedef						float   d_type;				  //���������������

struct peg
{
	CPoint*                           m_pt;               //�����
	d_type                       m_dRatioX;               //���Rectλ��X
	d_type                       m_dRatioY;               //���Rextλ��Y
};

typedef CList<peg, peg>               PEGS;               //���peg�����ݽṹ
typedef vector<d_type>           TRANSFORM;               //��ŻҶȱ任������,2015.5.16�޸�Ϊd_type����
typedef vector<d_type>              VECTOR;               //������ֵ������׷�Ϸ��Ĳ�������vector�����洢

class CCurveDlg;

class CCurveWnd : public CWnd
{
	DECLARE_DYNAMIC(CCurveWnd)

public:
	CCurveWnd();
	virtual ~CCurveWnd();

	CCurveDlg* GetDocument();							  /*��ȡ������*/
	CCTdemoView* GetView();								  /*��ȡ��ͼָ��*/
	CCTdemoDoc* GetMainDoc();							  /*��ȡ�ĵ�ָ��*/

	void SetImage(CImage* pImage);						  //����ͼ��ָ��
	BOOL GetHistogram();                                  //ȡ��ֱ��ͼ
	void InitPegs();									  //��ʼ��pegs
	void UpdateImageInfo();								  //����ͼ����Ϣ
	BOOL PegIsExist(CPoint& point);                       //��peg�Ƿ����
	POSITION AddNewPeg(CPoint& point);                    //�����µ�peg
	BOOL RePlacePeg(CPoint& point);                       //�ı�ĳ��peg
	BOOL UserWillDrag(CPoint& point);                     //�û�������һ��peg
	void SplineFunc();                                    //������ֵ����
	void PieceWiseFunc();                                 //�ֶ����Ժ���
	BOOL CursorIsInPaintRect(CPoint& point);              //����ڻ�ͼ����֮��
	CPoint TransPositionToDialog(CPoint& point);          //ת�����Ի���Ĭ�ϵ�����ϵ(���Ͻ������)
	CPoint TransPositionToRectangle(CPoint& point);       //ת������Rect���½�Ϊԭ�������ϵ
	BOOL GrayTransform();                                 //�����������Ҷȱ任
	void Threshold();                                     //�Գ���[0,255]�ĻҶȽ��д���
	d_type* PreTransform();                               //�Ա任ֵԤ����
	void InverseTransform();                              //ͼ��ת
	void ImshowImmediately();                             //����ˢ��ͼ��
	d_type GetCurrentGrayTransform(d_type gray);          //��ȡ��ǰ�Ҷ�ֵ�ı任ֵ
	void ShowGrayTransform();                             //��ֵ����ʾ�Ҷȱ任����

	void ReShapeButtons(int nItemID, int nButtonPosX1, int nButtonPosX2, d_type Space, int Number);
	void ReshapePegs(bool bAllRefresh = false);			  //ˢ��pegs, trueΪˢ��ȫ��, ����ˢ�µ�ǰ
	void ReshapeTransform(bool bAllRefresh = false);	  //ˢ��ͨ���任, trueΪˢ��ȫ��, ����ˢ�µ�ǰ

	/*** 2015��5�� ��� ***/
	bool m_bModified;
	int m_nNewRowlen;
	int m_nNewChannel;
	int m_nNewlenData;
	void ApplyToImage();								  //Ӧ�õ�ͼ��
	void MallocData();									  //���仺������
	bool DetectModified();								  //���ͼ���Ƿ����
	bool DetectImageChanged();							  //���ͼ���Ƿ�任

public:
	//��ť
	CComboBox*                         m_pComboBoxRGB;    //RGBͨ��:4
	CButton*                            m_pBnGroupBox;    //Group Box
	CMFCButton*                      m_pBnChooseCurve;    //����ģʽ
	CMFCButton*                       m_pBnChooseLine;    //ֱ��ģʽ
	CMFCButton*                          m_pBnInverse;    //��ת
	CMFCButton*                            m_pBnReset;    //����
	CMFCButton*                      m_pBnImportCurve;    //��������
	CMFCButton*                      m_pBnExportCurve;    //��������
	CMFCButton*                               m_pBnOK;    //ȷ��
	CMFCButton*                           m_pBnCancel;    //ȡ��
	CEdit*                               m_pXPosition;    //X����
	CEdit*                               m_pYPosition;    //Y����
	CStatic*                                m_pXlabel;    //X��ǩ
	CStatic*                                m_pYlabel;    //Y��ǩ
	CImage*                                  m_pImage;    //CImageָ��
	CFont*                                 m_pAppFont;    //����
	int                                   m_nFontSize;    //�����С
	CString                                m_FontName;    //������
	d_type*                          m_pfHistogram[4];    //ֱ��ͼ:4*256
	TRANSFORM                        m_V4Transform[4];    //�Ҷȱ任:4*m_nWidth
	PEGS                               m_pPegsList[4];    //ͼ��ѡ�ĵ�:4*m_nWidth
	CRect*                             m_pOnPaintRect;    //��ͼ����
	CRect*                                 m_pWndSize;    //��ǰWnd��С
	int                            m_nChannelSelected;    //��ǰͨ��
	BOOL                              m_bUserWillDrag;    //����peg
	int                                  m_nRectWidth;    //��ͼ�����
	int                                 m_nRectHeight;    //��ͼ���߶�
	d_type                              m_fWidthRatio;    //������ű�
	d_type                             m_fHeightRatio;    //�߶����ű�
	POSITION                          m_posCurrentPeg;    //��ǰק����peg
	int                                m_nLinePattern;    //����
	BOOL                              m_bInverseImage;    //��תͼ��

	// ͼ����Ϣ - ͼ����ܻ�仯�����ͼ����Ϣ��������Ϊ���Ա����.
	BYTE*                                     m_pBits;    //ͼ��ָ��
	BYTE*                                   m_dataSrc;    //ͼ��Դ����
	BYTE*                                   m_curData;    //��ǰͼ������
	long                                m_nImageWidth;    //ͼ����
	long                               m_nImageHeight;    //ͼ��߶�
	long                             m_nPixelQuantity;    //���ظ���
	long                                m_nbytesWidth;    //ͼ��λ��
	int                                    m_nChannel;    //ͼ��ͨ��
	DWORD                                  m_nlenData;    //ͼ�񳤶�

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

private://������˽�к���
	VECTOR Spline(VECTOR& Node, VECTOR& Value, VECTOR& x0, d_type c1, d_type c2, int CASE = 2);
	d_type* Spline(d_type* Node, d_type* Value, int Node_Num, d_type* x0, int x0_len, d_type c1, d_type c2, int CASE = 2);
	void Chase(VECTOR& a, VECTOR& b, VECTOR& c, VECTOR& f, VECTOR& x, int n);
	void Chase(d_type* a, d_type* b, d_type* c, d_type* f, d_type* x, int n);
	COLORREF ColorSetting(int Channel, int Strength, int WhereUse = 0);
};