#pragma once

/** 
* @file FileIO.h
* @brief �ļ���д���������Կ�ƽ̨
*/

#include "DataTypes.h"
#include "Transform.h"

/**
* @brief ������д���ļ�
* @param[in] *fileName �ļ���
* @param[in] *pSrc ����ָ��
* @param[in] nLength ���鳤��
* @return �ɹ�����TRUE
*/
template <typename Type> BOOL WriteArray(const char* fileName, const Type* pSrc, int nLength)
{
	ofstream fout;
	fout.open(fileName);

	if (fout.is_open())
	{
		try
		{
			for (int i = 0; i < nLength - 1; ++i)
			{
				fout << *(pSrc + i) << ",";
			}
			fout << *(pSrc + nLength - 1);
			fout.close();
			return TRUE;
		}
		catch(...)
		{
			fout.close();
			return FALSE;
		}
	}
	
	return FALSE;
}

/** 
* @brief ��BYTEͼ�������Զ�������ʽд���ļ�,nChannel��ʾһ������ռ�ü����ֽ�
* @param[in] *fileName �ļ���
* @param[in] *pSrc ͼ������
* @param[in] nWidth ͼ����
* @param[in] nHeight ͼ��߶�
* @param[in] nChannel ÿ�����ֽ�
*/
inline BOOL WriteTxt(const char* fileName, const BYTE* pSrc, int nWidth, int nHeight, int nChannel)
{
	return ImageWrite(fileName, pSrc, nWidth, nHeight, WIDTHBYTES(8 * nWidth * nChannel));
}

// ��ͼ�������Զ�������ʽ�����ļ���nChannel��ʾһ������ռ�ü����ֽ�
BYTE* ReadTxt(const char* fileName, int &nWidth, int &nHeight, int &nRowlen, int &nChannel);