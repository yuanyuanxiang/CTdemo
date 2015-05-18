#pragma once
#include "afxwin.h"


// CDlgProjectSettings 对话框

class CDlgProjectSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProjectSettings)

public:
	CDlgProjectSettings(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProjectSettings();

// 对话框数据
	enum { IDD = IDD_PROJECT_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditRaysNum;
	CEdit m_EditAnglesNum;
	int m_nRaysNum;
	int m_nAnglesNum;
	CEdit m_EditAngleRange;
	float m_fAngleRange;
	virtual void OnOK();
	CEdit m_EditSubPixel;
	float m_fSubPixel;
};
