#pragma once

#include "atlimage.h"
#include "Functions.h"
//
// ********** CyImage：继承自CImage ********** //
//

class CyImage : public CImage
{
public:
	// 派生类新增的成员变量
	float*						m_pfFloat;				//对应图像的浮点数
	float						m_fMaximum;				//浮点数据的最大值
	float						m_fMinimum;				//浮点数据的最小值

	// 父类的图像信息无法直接访问，故出此下策
	int							m_nyWidth;				//图像宽度
	int							m_nyHeight;				//图像高度
	int							m_nyRowlen;				//图像每行字节数
	int							m_nyRowlen2;			//每行浮点数
	int							m_nyBpp;				//图像位深度
	int							m_nyChannel;			//每像素字节数

	// 构造 / 析构
	CyImage();
	~CyImage();

	// 对派生类成员变量的操作
	void UpdateImageInfomation();											//更新图像信息
	float* GetFloatData();													//获取浮点数据
	bool SetFloatData(float* pSrc, int nRowlen, int nHeight);				//设置浮点数据
	float GetMaximum();														//获取最大值
	float GetMinimum();														//获取最小值
	float GetAt(float x, float y, int nCurChannel = 0);						//访问浮点数据
	float GetAt(int x, int y, int nCurChannel = 0);							//访问浮点数据
	float* Rotate(float angle, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Zoom(int NewWidth, int NewHeight);
	float Integrate(float &k, float &c, int nCurChannel = 0);
	void DirIntegrate(float* pDst, int nLength, float angle, float sub_pixel, int nCurChannel = 0);
	void Radon(float* pDst, float angles_separation, int nAnglesNum, float rays_separation, int nRaysNum, int nCurChannel = 0);

	// 数据拷贝
	void MemcpyByteToFloat();
	void MemcpyFloatToByte();

	// 获取图像信息
	BYTE* GetHeadAddress();
	BYTE* GetLineAddress(int LineID);
	int GetChannel();
	int GetRowlen();
	int GetDiagLength();
	bool InitFloatData();
	void GetInfomation(int &nWidth, int &nHeight);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP, int &nChannel);

	// 创建对象
	BOOL Create(int nWidth, int nHeight, int nBPP, DWORD dwFlags = 0) throw();
	HRESULT Load(LPCTSTR pszFileName) throw();
	HRESULT Save(LPCTSTR pszFileName, REFGUID guidFileType = GUID_NULL) const throw();
	bool Save(REFGUID guidFileType) const throw();
	bool Save(CWnd* pParentWnd = NULL, REFGUID guidFileType = GUID_NULL) const throw();
	BOOL Create(float* pSrc, int nWidth, int nHeight, int nRowlen, DWORD dwFlags = 0) throw();
	void Destroy() throw();

private:
	void ComputeMinMax();
};