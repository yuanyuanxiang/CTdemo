#pragma once
#include "Resource.h"


// CDlgRawDataSize �Ի���

class CDlgRawDataSize : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRawDataSize)

public:
	CDlgRawDataSize(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRawDataSize();

// �Ի�������
	enum { IDD = IDD_RAW_DATA_SIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nRawWidth;
	int m_nRawHeight;
	int m_nRawHeader;
};
