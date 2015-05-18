#pragma once


// CDlgImageInfomation 对话框

class CDlgImageInfomation : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImageInfomation)

public:
	CImage* m_pImage;
	void SetImage(CImage* pImage);
	void ShowInfomation();
	CDlgImageInfomation(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgImageInfomation();

// 对话框数据
	enum { IDD = IDD_DLG_IMAGE_INFOMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
