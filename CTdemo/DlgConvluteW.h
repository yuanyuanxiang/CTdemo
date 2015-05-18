#pragma once
#include "afxwin.h"


// CDlgConvluteW 对话框

class CDlgConvluteW : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConvluteW)

public:
	CDlgConvluteW(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgConvluteW();

// 对话框数据
	enum { IDD = IDD_CONVOLUTE_W };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditW;
	float m_fW;
};
