#pragma once
#include "Resource.h"
#include "afxwin.h"


// CDlgRawDataSize �Ի���

class CDlgRawDataSize : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRawDataSize)

public:
	CDlgRawDataSize(BOOL bRead = TRUE, CWnd* pParent = NULL);
	virtual ~CDlgRawDataSize();

// �Ի�������
	enum { IDD = IDD_RAW_DATA_SIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bReadRaw;
	int m_nRawWidth;
	int m_nRawHeight;
	int m_nRawHeader;
	CEdit m_EditRawWidth;
	CEdit m_EditRawHeight;
	CEdit m_EditRawHeader;
	virtual BOOL OnInitDialog();
};