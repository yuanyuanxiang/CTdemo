#pragma once
#include "DataTypes.h"

/** 
* @class ImageInfo
* @brief ͼ������Դ��Ϣ
* @details �����ݽṹ�����ͼ�����Ҫ��Ϣ, ͼ���ע������Ϣ.

* @author yuanyuanxiang
* @version 1.0
* @date 2017/5/14
*/
class ImageInfo
{
public:
	/**
	* @brief ����һ��ͼ��ṹ
	* @param[in] *pHead				ͼ������
	* @param[in] nWidth				ͼ����
	* @param[in] nHeight			ͼ��߶�
	* @param[in] nChannel			ͼ��ͨ��
	* @param[in] roi				����Ȥ����
	*/
	ImageInfo(const BYTE *pHead, int nWidth, int nHeight, int nChannel, 
		const RoiRect &roi = 0) : 
		m_pHead(pHead), 
		m_nWidth(nWidth), 
		m_nHeight(nHeight), 
		m_nRowlen(WIDTHBYTES(8 * nChannel * nWidth)),
		m_nChannel(nChannel),
		m_roi(roi),
		m_Background(0) { }

	ImageInfo() { memset(this, 0, sizeof(ImageInfo)); }

	~ImageInfo(void) { }

	/// �ļ�ͷ��ʶ
	static const char *m_sTag;
	/// ��ʼ��
	static void Init(const char *tag) { m_sTag = tag; }

public:
	/// ͼ������Դ
	const BYTE *m_pHead;
	/// ͼ����
	int m_nWidth;
	/// ͼ��߶�
	int m_nHeight;
	/// ͼ��ÿ��Ԫ�ظ���
	int m_nRowlen;
	/// ͼ��ÿ����Ԫ�ظ���
	int m_nChannel;
	/// ����Ȥ����
	RoiRect m_roi;
	/// ������ɫ
	COLORREF m_Background;
};


/**
* @class DecodeSrcInfo
* @brief ��ά����������Դ�ṹ

* @author yuanyuanxiang
* @version 1.0
* @date 2017/5/21
*/
class DecodeSrcInfo
{
public:
	/// ͼ������Դ
	ImageInfo m_imgSrc;
	/// QR��������1(�����ֵ��)
	BOOL m_bUseHybrid;
	/// QR��������2(ǿ������)
	BOOL m_bTryHarder;

	/** 
	* @brief ���캯��
	* @param[in] src		ͼ������Դ
	* @param[in] bUseHybrid	QR��������1
	* @param[in] bTryHarder	QR��������2
	*/
	DecodeSrcInfo(const ImageInfo & src, BOOL bUseHybrid, BOOL bTryHarder) :
		m_imgSrc(src), m_bUseHybrid(bUseHybrid), m_bTryHarder(bTryHarder)
	{
	}

	DecodeSrcInfo() { memset(this, 0, sizeof(DecodeSrcInfo)); }
};


/**
* @class ImageSrc
* @brief ͼ����
*/
class ImageSrc : public ImageInfo
{
private:
	BYTE *m_data;
public:
	ImageSrc(int nWidth, int nHeight, int nChannel):
		ImageInfo(NULL, nWidth, nHeight, nChannel)
	{
		m_data = new BYTE[m_nHeight * m_nRowlen * sizeof(BYTE)];
		m_pHead = m_data;
	}

	ImageSrc() { memset(this, 0, sizeof(ImageSrc)); }

	~ImageSrc() { SAFE_DELETE(m_data); }

	inline int GetWidth() const { return m_nWidth; }

	inline int GetHeight() const { return m_nHeight; }

	inline int GetRowlen() const { return m_nRowlen; }

	inline int GetChannel() const { return m_nChannel; }

	inline int GetBPP() const { return 8 * m_nChannel; }

	inline const BYTE* GetHeadAddress() const { return m_pHead; }

	inline BYTE* GetImage() const { return m_data; }
};