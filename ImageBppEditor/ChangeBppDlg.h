#pragma once

#include "afxwin.h"
#include <atlimage.h>
#include <afxdialogex.h>
#include "resource.h"

#define IDC_STATIC_BPP                  1
#define IDC_COMBO_BPP                   2

// CChangeBppDlg 对话框

// ************************************************************* //
// 关于DoModal返回-1的问题。请注意配置项目为"在静态库中使用 MFC" //
// 这样 FindResource 就从 dll 中寻找资源，否则从 exe 中寻找资源  //
// ****************** 2015年3月24日 备注 *********************** //

// 在计算图像大小时，采用公式：biSizeImage = biWidth' × biHeight。
// 是biWidth'，而不是biWidth，这里的biWidth'必须是4的整倍数，表示
// 大于或等于biWidth的，离4最近的整倍数。WIDTHBYTES就是用来计算
// biWidth'
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

#define SAFE_DELETE(p)		{ if (p != NULL) { delete [] p; p = NULL; } }

class CChangeBppDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeBppDlg)

public:
	CChangeBppDlg(CWnd* pParent = NULL);   // 标准构造函数
	CChangeBppDlg(CImage* pImage, CWnd* pParent = NULL);
	virtual ~CChangeBppDlg();

// 对话框数据
	enum { IDD = IDD_DLG_CHANGE_BPP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CImage*			m_pImage;							// 外部传入的指针
	CStatic*		m_pStaticBpp;						// 显示当前位深度
	CComboBox*		m_pComboBpp;						// 位深度复选框
	CButton*		m_pBnOK;							// Ok 按钮
	CButton*		m_pBnCancel;						// Cancel 按钮
	CFont			m_Font;								// 控件的字体
	int				m_nTargetBpp;						// 目标图像位深度
	BOOL			ChangeBPP(UINT bpp);				// 改变图像通道数
	afx_msg void	OnSelchangeComboBpp();				// 改变位深度
	afx_msg void	OnBnClickedOk();					// “OK”响应函数
	afx_msg void	OnBnClickCancel();					// "Cancel"响应函数
	virtual BOOL	OnInitDialog();						// 初始化动作
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);// 创新时执行

private:
	void	Bpp8To24();									// 8位转24位
	void	Bpp8To32();									// 8位转32位
	void	Bpp24To8();									// 24位转8位
	void	Bpp24To32();								// 24位转32位
	void	Bpp32To8();									// 32位转8位
	void	Bpp32To24();								// 32位转24位
	void	Bpp1To8();									// 掩码转8位
	void	Bpp1To24();									// 掩码转24位
	void	Bpp1To32();									// 掩码转32位
};


void SafeDestroy(CImage* pImage);						// 安全删除关联位图