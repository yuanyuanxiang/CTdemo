#pragma once
#include "Resource.h"


// CDlgRawDataSize 对话框

class CDlgRawDataSize : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRawDataSize)

public:
	CDlgRawDataSize(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRawDataSize();

// 对话框数据
	enum { IDD = IDD_RAW_DATA_SIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nRawWidth;
	int m_nRawHeight;
	int m_nRawHeader;
};
