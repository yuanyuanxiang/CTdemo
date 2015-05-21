#pragma once

#include "atlimage.h"
#include "Functions.h"

//
// ********** CyImage：继承自CImage *********** //
// ***** 首都师范大学，袁沅祥，2015年5月. ***** //
// 

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }	//安全删除指针p
#endif

#ifndef CHECK_IMAGE_NULL
#define CHECK_IMAGE_NULL(p) ( ((p) == NULL) || ((p)->IsNull()) ) //检查图像是空
#endif

#ifndef RAD
#define RAD(i) (PI * (i) / 180.0f)	//角度转弧度
#endif // !RAD

class CyImage : public CImage
{
public:
	// 派生类新增的成员变量
	float*						m_pfFloat;				//对应图像的浮点数
	float						m_fMaximum;				//浮点数据的最大值
	float						m_fMinimum;				//浮点数据的最小值

	// 父类的图像信息无法直接访问，故出此下策
	BYTE*						m_pyBits;				//图像首地址
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
	void UpdateInfomation();												//更新图像信息
	float* GetFloatData();													//获取浮点数据
	bool SetFloatData(float* pSrc, int nRowlen, int nHeight);				//设置浮点数据
	float GetMaximum();														//获取最大值
	float GetMinimum();														//获取最小值
	float GetAt(float x, float y, int nCurChannel = 0);						//访问浮点数据
	float GetAt(int x, int y, int nCurChannel = 0);							//访问浮点数据

	// 图像几何变换
	void FlipH();															//水平翻转图像
	void FlipV();															//垂直翻转图像
	void Rotate(float degree);												//图像按角度旋转
	void Zoom(float rate);													//图像按倍数缩放
	void Zoom(int nNewWidth, int nNewHeight);								//图像按像素缩放
	/* 对图像旋转，返回新图像的数据，宽度、高度和每行浮点数 */
	float* Rotate(float angle, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &NewWidth, int &NewHeight, int &NewRowlen);
	float* Rotate(float angle, float x0, float y0, int &Xmin, int &Ymin, int &Xmax, int &Ymax, int &NewWidth, int &NewHeight, int &NewRowlen);
	/* 根据参数放大图像，返回浮点数据 */
	float* Zoom(int NewWidth, int NewHeight, int Reserved);
	/* 图像沿直线作线积分 */
	float Integrate(float &k, float &c, int nCurChannel = 0);
	/* 图像在某个方向所有线积分 */
	float* DirIntegrate(float angle, int nCurChannel = 0);
	/* 图像radon变换 */
	float* Radon(float angles_separation, int nAnglesNum, float pixels_separation, int nRaysNum, int nCurChannel = 0);

	// 字符串数据和浮点数据之间拷贝，同步数据时候需调用
	void MemcpyByteToFloat();												//字符数据拷贝到浮点数据
	void MemcpyFloatToByte();												//浮点数据拷贝到字符数据
	void MemcpyFloatToByteBounded(float lower, float upper);				//根据上下限拷贝数据

	// 获取图像信息
	bool BitMapModified();													//检查关联位图是否更新
	BYTE* GetHeadAddress();													//获取图像首地址
	BYTE* GetLineAddress(int LineID);										//获取某行的地址
	int GetChannel();														//获取通道个数
	int GetRowlen();														//获取每行字节数
	int GetDiagLength();													//获取图像对角线长
	bool InitFloatData();													//初始化浮点数据
	/* 获取图像的信息 */
	void GetInfomation(int &nWidth, int &nHeight);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP);
	void GetInfomation(int &nWidth, int &nHeight, int &nRowlen, int &nBPP, int &nChannel);

	/* 修改图像位深度*/
	BOOL	ChangeBPP(UINT bpp);						// 改变图像通道数
	void	Bpp8To24();									// 8位转24位
	void	Bpp8To32();									// 8位转32位
	void	Bpp24To8();									// 24位转8位
	void	Bpp24To32();								// 24位转32位
	void	Bpp32To8();									// 32位转8位
	void	Bpp32To24();								// 32位转24位
	void	Bpp1To8();									// 掩码转8位
	void	Bpp1To24();									// 掩码转24位
	void	Bpp1To32();									// 掩码转32位

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