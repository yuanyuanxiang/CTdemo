#pragma once

/**
* @file templateFuncs.h
* @brief ����ģ��
* @details ��Ҫ������1��ͼ��˫���Բ�ֵ������2����ͼ���ȡ��������
*		3���ֽ����͸��������໥������4��������ݿ�������Сֵ��
*/

#include "DataTypes.h"

// ��ȡ����(x, y)����ֵ
template <typename Type> inline float GetPositionValue(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
													   int nCurChannel, int x, int y);

// ��ֵ������(x, y)����ֵ,˫���Բ�ֵ
template <typename Type> float biLinearInterp(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
											  int nCurChannel, float x, float y);

// ��ͼ���ȡ���������ڽ���
template <typename Type> Type* GetDecodeString(const Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// ��Byte(uchar)���ݿ�����������
template <typename Type> void MemcpyByte2Float(float* pDst, const Type* pSrc, int nWidth, int nHeight, int nChannel);

// ���������ݿ�����BYTE(uchar)
template <typename Type> void MemcpyFloat2Byte(Type* pDst, const float* pSrc, int nWidth, int nHeight, int nChannel);


/** 
* @brief ��ȡ����(x, y)���ĺ���ֵ
* @param[in] *pSrc			ͼ������
* @param[in] nWidth			ͼ����
* @param[in] nHeight		ͼ��߶�
* @param[in] nRowlen		ͼ��ÿ���ֽ���
* @param[in] nChannel		ͼ��ͨ��
* @param[in] nCurChannel	��ǰͨ��
* @param[in] x				��ֵ����
* @param[in] y				��ֵ����
*/
template <typename Type> inline float GetPositionValue(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
													   int nCurChannel, int x, int y)
{
	if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
		return 0.0F;

	return float(*(pSrc + nCurChannel + x * nChannel + y * nRowlen));
}


/** 
* @brief ˫���Բ�ֵ��ȡ����(x, y) ���ĺ���ֵ
* @param[in] *pSrc			ͼ������
* @param[in] nWidth			ͼ����
* @param[in] nHeight		ͼ��߶�
* @param[in] nRowlen		ͼ��ÿ���ֽ���
* @param[in] nChannel		ͼ��ͨ��
* @param[in] nCurChannel	��ǰͨ��
* @param[in] x				��ֵ����
* @param[in] y				��ֵ����
*/
template <typename Type> inline float biLinearInterp(const Type *pSrc, int nWidth, int nHeight, int nRowlen, int nChannel, 
											  int nCurChannel, float x, float y)
{
	/** 
	// ˫���Բ�ֵ��ʾ��ͼ:
	// Ans2��		(x4, y4)				(x3, y3)
	//							(x, y)
	// Ans1��		(x1, y1)				(x2, y2)
	*/
	int x1 = int(x), y1 = int(y), x3 = x1 + 1, y3 = y1 + 1;
	// ���½ǵĵ�
	const Type* pLB = pSrc + nCurChannel + x1 * nChannel + y1 * nRowlen;
	// ��Խ��Ĵ���
	return (x1 < 0 || x3 >= nWidth || y1 < 0 || y3 >= nHeight) ? 0 : 
		(*pLB * (x3 - x) + *(pLB + nChannel) * (x - x1)) * (y3 - y) 
		+ (*(pLB + nRowlen) * (x3 - x) + *(pLB + nChannel + nRowlen) * (x - x1)) * (y - y1);
}


/**
* @brief ��ý���������
* @param[in] *pHead			ͼ������
* @param[in] nWidth			ͼ����
* @param[in] nHeight		ͼ��߶�
* @param[in] nRowlen		ͼ��ÿ���ֽ���
@return 4ͨ����ͼ������(�뽫��תΪchar�����ڽ���)
@note Type����Ϊchar��BYTE
*/
template <typename Type> Type* GetDecodeString(const Type* pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	ASSERT(sizeof(Type) == 1);
	// ����ͼ�������λ��
	const int nBPP = nRowlen / nWidth * 8;
	// 4ͨ��ͼ���ÿ���ֽ���
	const int NewRowlen = nWidth * 4;
	// Ϊ����ֵ�����ڴ�
	Type *pDst = new Type[nHeight * NewRowlen];
	switch (nBPP)
	{
	case 8:// GRAY
		{
			const Type* pSrc0 = pHead;
			Type* pCur0 = pDst;
			for (int i = 0; i < nHeight; i++)
			{
				const Type* pSrc = pSrc0;
				Type* pCur = pCur0;
				for (int j = 0; j < nWidth; j++)
				{
					* pCur = *(pCur+1) = *(pCur+2) = *pSrc;
					*(pCur+3) = 0;// Alpha
					pSrc ++;
					pCur += 4;
				}
				pSrc0 += nRowlen;
				pCur0 += NewRowlen;
			}
		}
		break;
	case 24:// RGB
		{
			const Type* pSrc0 = pHead;
			Type* pCur0 = pDst;
			for (int i = 0; i < nHeight; i++)
			{
				const Type* pSrc = pSrc0;
				Type* pCur = pCur0;
				for (int j = 0; j < nWidth; j++)
				{
					* pCur = *pSrc;
					*(pCur+1) = *(pSrc+1);
					*(pCur+2) = *(pSrc+2);
					*(pCur+3) = 0;// Alpha
					pSrc += 3;
					pCur += 4;
				}
				pSrc0 += nRowlen;
				pCur0 += NewRowlen;
			}
		}
		break;
	case 32:// RGBA
		memcpy(pDst, pHead, nHeight * NewRowlen * sizeof(Type));
		break;
	default:
		SAFE_DELETE(pDst);
		return NULL;
	}
	return pDst;
}


/** 
* @brief ���������ݿ�����BYTE. 
* @param[in] *pDst			�ֽ�����
* @param[in] *pSrc			��������
* @param[in] nWidth			ͼ����
* @param[in] nHeight		ͼ��߶�
* @param[in] nChannel		ͼ��ͨ��
*/
template <typename Type> void MemcpyFloat2Byte(Type* pDst, const float* pSrc, int nWidth, int nHeight, int nChannel)
{
	// BYTEÿ���ֽ���
	const int nRowlen = WIDTHBYTES(nWidth * nChannel * 8);
	// ��������ÿ�и���
	const int nFloatRowlen = nWidth * nChannel;

	for (int i = 0; i < nWidth; ++i)
	{
		int x = i * nChannel;
		for (int j = 0; j < nHeight; ++j)
		{
			int y1 = j * nFloatRowlen, y2 = j * nRowlen;
			for (int k = 0; k < nChannel; ++k)
				pDst[k + x + y2] = (Type)pSrc[k + x + y1];
		}
	}
}


/** 
* @brief ��BYTE��������������
* @param[in] *pDst			��������
* @param[in] *pSrc			�ֽ�����
* @param[in] nWidth			ͼ����
* @param[in] nHeight		ͼ��߶�
* @param[in] nChannel		ͼ��ͨ��
*/
template <typename Type> void MemcpyByte2Float(float* pDst, const Type* pSrc, int nWidth, int nHeight, int nChannel)
{
	// BYTE����ÿ���ֽ���
	const int nRowlen = WIDTHBYTES(nWidth * nChannel * 8);
	// ��������ÿ�и���
	const int nFloatRowlen = nWidth * nChannel;

	for (int i = 0; i < nWidth; ++i)
	{
		int x = i * nChannel;
		for (int j = 0; j < nHeight; ++j)
		{
			int y1 = j * nRowlen, y2 = j * nFloatRowlen;
			for (int k = 0; k < nChannel; ++k)
				pDst[k + x + y2] = (float)pSrc[k + x + y1];
		}
	}
}