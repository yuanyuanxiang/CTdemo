/** 
* @file Transform.h
* @brief ͼ��任ģ�庯��,���Կ�ƽ̨.
* @details ������ͼ�����š���ת����ת������ͼ�����Ȥ���򡢱��桢ֱ��ͼ������
* @author Ԭ����
* @email 962914132@qq.com
* @version V1.0
* @date 2016/10/4
* Copyleft (c), All rights reserved.
* @note �����Ӵ˰���"algorithm.h"�ļ���������Eclipse����
* error: 'sort' is not a member of 'std'��
* ������**����ָ����Ϊ�������������޸Ĵ�һ��ָ��
*/

#include "DataTypes.h"
#include <fstream>
using namespace std;

#pragma once

#pragma warning(disable : 4244)

// ��ͼ��д���ļ����ļ�ͷ��С���ֽڣ�
#define HEADER_SIZE 1024

// �Ŵ�ͼ�񣺻��޸�pfHead������ͼ����Ϣ
template <typename Type> void ZoomImage(Type** pfHead, int &nWidth, int &nHeight, int &nRowlen, int nChannel, int NewWidth, int NewHeight);

// ����ͼ�����߳���������ʱ�޸Ĵ�С
template <typename Type> void LimitImage(Type** pfHead, int &nWidth, int &nHeight, int &nRowlen, int nChannel, int nMaxSize = 320);

// ���·�תͼ��
template <typename Type> void ImageFlipV(Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// ���ҷ�תͼ��
template <typename Type> void ImageFlipH(Type* pHead, const int nWidth, const int nHeight, const int nRowlen);

// ת��ͼ��
template <typename Type> void ImageTranspose(Type** pHead, const int nWidth, const int nHeight, const int nRowlen);

// ��ȡͼ�����Ȥ����
template <typename Type> Type* ImageROI(const Type* pHead, int &nWidth, int &nHeight, int &nRowlen, const RoiRect &roi = 0);

// ��ͼ���Զ�������ʽд���ļ�
template <typename Type> BOOL ImageWrite(const char* fileName, const Type* pSrc, int nWidth, int nHeight, int nRowlen);

// ��ɫͼ��תΪ8λ�ĵ�ͨ��ͼ��
template <typename Type> Type* Rgb2Gray(const Type* pHead, int nWidth, int nHeight, int nRowBytes);

// ͳ�ƻҶ�ͼ���ֱ��ͼ
template <typename Type> void GrayHistogram(const Type* pHead, int nRowlen, int nHist[256], const RoiRect &roi);

// ͳ��ͼ���ֱ��ͼ
template <typename Type> void ImageHistogram(const Type* pHead, int nWidth, int nHeight, int nRowlen, int nHist[256]);

//////////////////////////////////////////////////////////////////////////


/** 
* @brief ���·�תͼ��.
* @param[in] *pHead ����ͷ
* @param[in] nWidth ���
* @param[in] nHeight �߶�
* @param[in] nRowlen ÿ�и���
@return void
*/
template <typename Type> void ImageFlipV(Type* pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	// ��������
	Type* temp = new Type[nHeight * nRowlen * sizeof(Type)];
	memcpy(temp, pHead, nHeight * nRowlen * sizeof(Type));

	const Type *pSrc = temp + (nHeight - 1) * nRowlen;
	Type *pDst = pHead;
	for (int r = 0; r < nHeight; ++r)
	{
		memcpy(pDst, pSrc, nRowlen * sizeof(Type));
		pSrc -= nRowlen;
		pDst += nRowlen;
	}
	SAFE_DELETE(temp);
}


/** 
* @brief ���ҷ�תͼ��
* @param[in] *pHead ����ͷ
* @param[in] nWidth ���
* @param[in] nHeight �߶�
* @param[in] nRowlen ÿ�и���
*/
template <typename Type> void ImageFlipH(Type* pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	Type* temp = new Type[nHeight * nRowlen];
	memcpy(temp, pHead, nHeight * nRowlen * sizeof(Type));

	const int nChannel = nRowlen / nWidth;
	const Type *pSrc = temp + (nWidth - 1) * nChannel;
	Type *pDst = pHead;
	for (int r = 0; r < nHeight; ++r)
	{
		const Type *pSrcLine = pSrc;// ��ָ��
		Type *pDstLine = pDst;
		for (int c = 0; c < nWidth; ++c)
		{
			memcpy(pDstLine, pSrcLine, nChannel * sizeof(Type));
			pSrcLine -= nChannel;
			pDstLine += nChannel;
		}
		pSrc += nRowlen;
		pDst += nRowlen;
	}
	SAFE_DELETE(temp);
}


/** 
* @brief ���ҷ�תͼ��
* @param[in] **pHead ����ͷ
* @param[in] nWidth ���
* @param[in] nHeight �߶�
* @param[in] nRowlen ÿ�и���
* @note �������޸�ͼ��ָ��
*/
template <typename Type> void ImageTranspose(Type** pHead, const int nWidth, const int nHeight, const int nRowlen)
{
	// ͼ��ÿ����Ԫ�ظ���
	const int nChannel = nRowlen / nWidth;
	// ÿ��Ԫ�ظ���
	int nNewRowlen = nHeight * nChannel;
	// �Ƿ���Ҫ���ж��봦��
	if (1 == sizeof(Type))
	{
		nNewRowlen = WIDTHBYTES(nNewRowlen * 8);
	}

	Type *pSrc = *pHead;
	Type *temp = new Type[nWidth * nNewRowlen];
	for (int k = 0; k < nChannel; ++k)
	{
		int x = 0, y1 = 0;
		for (int i = 0; i < nWidth; ++i)
		{
			int y = 0, x1 = 0;
			for (int j = 0; j < nHeight; ++j)
			{
				temp[k + x1 + y1] = pSrc[k + x + y];
				y += nRowlen;
				x1 += nChannel;
			}
			x += nChannel;
			y1 += nNewRowlen;
		}
	}
	delete[] * pHead;
	*pHead = temp;
	temp = NULL;
}


/** 
* @brief ��ȡͼ�����Ȥ����
* @param[in] **pHead ����ͷ
* @param[in] &nWidth ���
* @param[in] &nHeight �߶�
* @param[in] &nRowlen ÿ�и���
* @param[in] &roi ͼ�����Ȥ����
* @return ��������ͼ�����Ȥ����(ָ�룬��ҪSAFE_DELETE)
* @note ��������ͼ���ȡ��߶ȵ���Ϣ�����ROI�Ŀ�ȡ��߶ȵ���Ϣ
*/
template <typename Type> Type* ImageROI(const Type* pHead, int &nWidth, int &nHeight, int &nRowlen, const RoiRect &roi)
{
	/// ���roi�Ƿ�
	if (roi.Width() <= 0 || roi.Height() <= 0 || roi.right > nWidth || roi.bottom > nHeight)
	{
		Type* pDst = new Type[nHeight * nRowlen];
		memcpy(pDst, pHead, nHeight * nRowlen * sizeof(Type));
		return pDst;
	}

	const int nChannel = nRowlen / nWidth;
	const int nNewWidth = roi.Width();
	const int nNewHeight = roi.Height();
	int nNewRowlen = nNewWidth * nChannel;
	// �Ƿ���Ҫ���ж��봦��
	if (1 == sizeof(Type))
	{
		nNewRowlen = WIDTHBYTES(nNewRowlen * 8);
	}

	Type* pDst = new Type[nNewHeight * nNewRowlen];

	Type *pDstLine = pDst;
	const Type *pSrcLine = pHead + roi.left * nChannel + (nHeight - roi.top - 1) * nRowlen;
	for (int i = 0; i < nNewHeight; ++i)
	{
		memcpy(pDstLine, pSrcLine, nNewRowlen * sizeof(Type));
		pSrcLine -= nRowlen;
		pDstLine += nNewRowlen;
	}

	// ����ͼ����Ϣ
	nWidth = nNewWidth;
	nHeight = nNewHeight;
	nRowlen = nNewRowlen;
	return pDst;
}


/** 
* @brief ��ͼ���Զ�������ʽд���ļ�
* @param[in] *fileName �ļ�����
* @param[in] *pHead ����ͷ
* @param[in] nWidth ���
* @param[in] nHeight �߶�
* @param[in] nRowlen ÿ�и���
* @see FileIO�е�WriteTxt/ReadTxt����
*/
template <typename Type> BOOL ImageWrite(const char* fileName, const Type* pSrc, int nWidth, int nHeight, int nRowlen)
{
	ofstream fout;
	fout.open(fileName, ofstream::binary);

	if (fout.is_open())
	{
		try
		{
			// д�ļ�ͷ����СHEADER_SIZE��
			char* pHead = "yuanyuanxiang";
			int nLen = strlen(pHead);
			fout.write(pHead, nLen);
			int nChannel = nRowlen / nWidth;
			int nInt = sizeof(int);
			fout.write((char*)&nRowlen, nInt);
			fout.write((char*)&nHeight, nInt);
			fout.write((char*)&nChannel, nInt);
			fout.seekp(HEADER_SIZE);
			// дͼ������
			fout.write((char*)pSrc, nHeight * nRowlen);
			fout.close();
			return TRUE;
		}
		catch (...)
		{
			fout.close();
			return FALSE;
		}
	}
	return FALSE;
}


/* - �Ŵ�ͼ�� -
* @param[in] **pHead ����ͷ
* @param[in] &nWidth ���
* @param[in] &nHeight �߶�
* @param[in] &nRowlen ÿ�и���
* @param[in] nChannel ͼ��ͨ��
* @param[in] NewWidth ͼ���¿��
* @param[in] NewHeight ͼ���¸߶�
* @note �������޸�pfHeadָ������ݣ���Ϊ���
*/
template <typename Type> void ZoomImage(Type** pfHead, int &nWidth, int &nHeight, int &nRowlen, int nChannel, int NewWidth, int NewHeight)
{
	if (nWidth == NewWidth && nHeight == NewHeight)
		return;

	int NewRowlen = NewWidth * nChannel;
	// �Ƿ���Ҫ���ж��봦��
	if (1 == sizeof(Type))
	{
		NewRowlen = WIDTHBYTES(NewRowlen * 8);
	}

	Type* pDst = new Type[NewRowlen * NewHeight];
	float wRatio = 1.f * nWidth / NewWidth;
	float hRatio = 1.f * nHeight / NewHeight;

	for (int nCurChannel = 0; nCurChannel < nChannel; ++nCurChannel)
	{
		int x = 0;
		float s = 0;
		for (int i = 0; i < NewWidth; ++i)
		{
			int y = 0;
			float t = 0;
			for (int j = 0; j < NewHeight; ++j)
			{
				int x1 = int(s), y1 = int(t), x3 = x1 + 1, y3 = y1 + 1;
				// ���½ǵĵ�
				const Type* pLB = *pfHead + nCurChannel + x1 * nChannel + y1 * nRowlen;
				// ��Խ��Ĵ���
				pDst[nCurChannel + x + y] = (x1 < 0 || x3 >= nWidth || y1 < 0 || y3 >= nHeight) ? 0 : 
					(*pLB * (x3 - s) + *(pLB + nChannel) * (s - x1)) * (y3 - t) 
					+ (*(pLB + nRowlen) * (x3 - s) + *(pLB + nChannel + nRowlen) * (s - x1)) * (t - y1);
				y += NewRowlen;
				t += hRatio;
			}
			x += nChannel;
			s += wRatio;
		}
	}

	// �޸�pHeadָ�������
	delete[] * pfHead;
	*pfHead = pDst;
	pDst = NULL;

	// ����ͼ����Ϣ
	nWidth = NewWidth;
	nHeight = NewHeight;
	nRowlen = NewRowlen;
}


/* - ����ͼ�����߳� -
* @param[in] **pHead ����ͷ
* @param[in] &nWidth ���
* @param[in] &nHeight �߶�
* @param[in] &nRowlen ÿ�и���
* @param[in] nChannel ͼ��ͨ��
* @param[in] nMaxSize ���߳�320
* @note ��ͼ����һ�߳��ȳ�������ʱ�������Ų�������Ϣ
* @warning ��ȡ��߶ȡ�ÿ���ֽ����ᱻ�޸�
*/
template <typename Type> void LimitImage(Type** pfHead, int &nWidth, int &nHeight, int &nRowlen, int nChannel, int nMaxSize)
{
	float rate = 0.0f;
	if (nWidth > nHeight)
	{
		if (nWidth > nMaxSize)
		{
			rate = 1.f * nMaxSize / nWidth;
			ZoomImage(pfHead, nWidth, nHeight, nRowlen, nChannel, nMaxSize, int(rate * nHeight));
		}
	}
	else
	{
		if (nHeight > nMaxSize)
		{
			rate = 1.f * nMaxSize / nHeight;
			ZoomImage(pfHead, nWidth, nHeight, nRowlen, nChannel, int(rate * nWidth), nMaxSize);
		}
	}
}

/** 
* @brief ��ɫת�ڰ�.
* @param[in] *pHead ͼ������
* @param[in] nWidth ͼ����
* @param[in] nHeight ͼ��߶�
* @param[in] nRowBytes ÿ���ֽ���
* @return ���������ػҶ�ͼ��
* @note ���޶�char/BYTE���д���
* @warning ��Ҫ�Է���ֵ����delete����.
*/
template <typename Type> Type* Rgb2Gray(const Type* pHead, int nWidth, int nHeight, int nRowBytes)
{
	ASSERT(1 == sizeof(Type));
	const int nChannel = nRowBytes / nWidth;
	const int nChannelNew = 1;
	const int nRowLenNew = WIDTHBYTES(nWidth * 8);
	Type *pDst = new Type[nHeight * nRowLenNew];

	switch (nChannel)
	{
	case 1:
		memcpy(pDst, pHead, nHeight * nRowBytes);
		break;
	case 3:
	case 4:// 3��4ͨ��������һ����
		{
			const Type *pSrcTemp0 = pHead;
			Type *pDstTemp0 = pDst;
			for (int i = 0; i < nHeight; i++)
			{
				const Type *pSrcTemp = pSrcTemp0;
				Type *pDstTemp = pDstTemp0;
				for (int j = 0; j < nWidth; j++)
				{
					// ����ǿ��ת��Ϊ�޷����ַ�������(2016/9/12 ע)
					BYTE R = *(pSrcTemp + 2);
					BYTE G = *(pSrcTemp + 1);
					BYTE B = *pSrcTemp;
					*pDstTemp = BYTE(RGB2GRAY(R, G, B));
					pSrcTemp += nChannel;
					pDstTemp += nChannelNew;
				}
				pSrcTemp0 += nRowBytes;
				pDstTemp0 += nRowLenNew;
			}
		}
		break;
	default:
		SAFE_DELETE(pDst);
		return NULL;
	}
	return pDst;
}


/** 
* @brief ͳ�ƻҶ�ͼ���ֱ��ͼ.
* @param[in] *pHead ͼ��ָ��
* @param[in] nRowlen ͼ��ÿ���ֽ���
* @param[in] nHist ͼ��ֱ��ͼ
* @param[in] &roi ͼ�����Ȥ����
*/
template <typename Type> void GrayHistogram(const Type* pHead, int nRowlen, int nHist[256], const RoiRect &roi)
{
	ASSERT(sizeof(Type) == 1);
	memset(nHist, 0, 256 * sizeof(int));
	const BYTE *pSrcLine = (const BYTE*)pHead + roi.left + roi.top * nRowlen;
	for (int i = roi.top; i < roi.bottom; ++i)
	{
		const BYTE *pCur = pSrcLine;
		for (int j = roi.left; j < roi.right; ++j)
		{
			BYTE cur = *pCur ++;
			++ nHist[cur];
		}
		pSrcLine += nRowlen;
	}
}

/** 
* @brief ͳ��ͼ���ֱ��ͼ.
* @param[in] *pHead ͼ��ָ��
* @param[in] nWidth ͼ����
* @param[in] nHeight ͼ��߶�
* @param[in] nRowlen ͼ��ÿ���ֽ���
* @param[in] nHist ͼ��ֱ��ͼ
*/
template <typename Type> 
void ImageHistogram(const Type* pHead, int nWidth, int nHeight, int nRowlen, int nHist[256])
{
	const int nChannel = nRowlen / nWidth;
	if (1 == nChannel)
		return GrayHistogram(pHead, nRowlen, nHist, RoiRect(0, 0, nWidth, nHeight));
	ASSERT(sizeof(Type) == 1);
	memset(nHist, 0, 256 * sizeof(int));
	const BYTE *pSrcLine = (const BYTE*) pHead;
	for (int i = 0; i < nHeight; ++i)
	{
		const BYTE *pCur = pSrcLine;
		for (int j = 0; j < nWidth; ++j)
		{
			int index = RGB2GRAY(*(pCur+2), *(pCur+1), *pCur);
			++ nHist[index];
			pCur += nChannel;
		}
		pSrcLine += nRowlen;
	}
}