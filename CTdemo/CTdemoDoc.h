
// CTdemoDoc.h : CCTdemoDoc ��Ľӿ�
//

#pragma once
#include "CyImage.h"

class CCTdemoView;
class CCurveView;
class CChildFrame;

#define PROJECT_TYPE_PAR	0		//ƽ����
#define PROJECT_TYPE_PAN	1		//������

class CCTdemoDoc : public CDocument
{
protected: // �������л�����
	CCTdemoDoc();
	DECLARE_DYNCREATE(CCTdemoDoc)

// ����
public:
	// ͶӰ����
	int					m_nProjectionType;	//ͶӰ����
	// �ļ���Ϣ
	CString				m_strFilePath;		//�ļ�·��
	CString				m_strFileName;		//�ļ�����
	CString				m_strFilePostfix;	//�ļ���׺
	// ָ�����
	CyImage*			m_pImage;			//ԭʼͼ��
	CyImage*			m_pProject;			//ͶӰͼ��
	CyImage*			m_pAfterFilter;		//�˲����ͶӰ
	CyImage*			m_pReconstruct;		//�ؽ�ͼ��
	BYTE*				m_pHead;			//ԭͼ�׵�ַ
	// ͼ����Ϣ
	CPoint				m_ptOrigin;			//ͼƬ����
	float				m_Ox;				//ͼƬ����x����
	float				m_Oy;				//ͼƬ����y����
	int					m_nWidth;			//ͼ����
	int					m_nHeight;			//ͼ��߶�
	int					m_nRowlen;			//ÿ���ֽ���
	int					m_nBPP;				//ÿ����λ��
	int					m_nChannel;			//ͼ��ͨ����
	int 				m_nImageDiag;		//ͼ��Խ��߳�
	// ������Ϣ
	float				m_fSubPixel;		//�����ؼ�
	float				m_fAnglesRange;		//�����ķ�Χ
	int					m_nAnglesNum;		//�����Ƕȸ���
	int					m_nRaysNum;			//ÿ���Ƕ�������
	float				m_fRaysSeparation;	//���߼��
	float				m_fAnglesSeparation;//�Ƕȼ��
	int					m_nDetectorCenter;	//̽��������

// ����
public:
	void				Popup(CyImage* pImage);								//����ͼ��Ԥ���Ի���
	void				UpdateImageInfomation();							//����ͼ����Ϣ
	void				InitScanningParameters();							//��ʼ����������
	CString				GetFileNameFromPath(CString path);					//��·��ȡ���ļ���
	CView*				GetView(CRuntimeClass* pClass);						// ��ȡ��ͼ
	CCTdemoView*		GetMainView();										// ��ȡ����ͼ
	CChildFrame*		GetChildFrame();									// ��ȡ�ӿ��
	CString				GetExeFilePath();									// ��ȡ��ǰexe�ļ�·��
	BOOL				OpenProjectionFile(LPCTSTR lpszPathName);			// ��ͶӰ�ļ�
	void				ReconstructImage(CString path);						// �����ݽ����ؽ�
	void				SetReconstructImageSize();							// �����ؽ�ͼ���С

	// *** ƽ������� ***
	void				RadonTransform();									// ����ͶӰͼ��

	// *** ��������� ***
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

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CCTdemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
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