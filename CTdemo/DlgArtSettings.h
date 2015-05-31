#pragma once
#include "afxwin.h"


// CDlgArtSettings 对话框

class CDlgArtSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgArtSettings)

public:
	CDlgArtSettings(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgArtSettings();

// 对话框数据
	enum { IDD = IDD_ART_IT_NUM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditArtItNum;
	int m_nArtItNum;
};
