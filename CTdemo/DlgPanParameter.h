#pragma once
#include "afxwin.h"


// CDlgPanParameter 对话框

class CDlgPanParameter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPanParameter)

public:
	CDlgPanParameter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPanParameter();

// 对话框数据
	enum { IDD = IDD_PAN_SCAN_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditAngleRange;
	float m_fAngleRange;
	CEdit m_EditRaysNum;
	int m_nRaysNum;
	CEdit m_EditSubPixel;
	float m_fSubPixel;
	CEdit m_EditAnglesNum;
	int m_nAnglesNum;
	CEdit m_EditPanSOR;
	float m_fPanSOR;
	CEdit m_EditPanSOD;
	float m_fPanSOD;
};
