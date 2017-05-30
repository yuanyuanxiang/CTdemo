#include "stdafx.h"
#include <cmath>
#include <algorithm>
#include "DataTypes.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/** �Ŵ���� */
void ZoomRect(CLogoRect &rect, float rate)
{
	rect.left = int(rate * rect.left);
	rect.top = int(rate * rect.top);
	rect.right = int(rate * rect.right);
	rect.bottom = int(rate * rect.bottom);
}


/** �ƶ����� */
void MoveRect(CLogoRect &rect, float dx, float dy)
{
	rect.left = int(rect.left + dx);
	rect.top = int(rect.top + dy);
	rect.right = int(rect.right + dx);
	rect.bottom = int(rect.bottom + dy);
}


/** 
* @brief �����ݽ����������
* @param[in] *nCodes		����������
* @param[in] *nMaskingNo	��������
* @param[in] nLength		���ݳ���
* @note XOR����ʵ�ֶ����ݵĳ������ܡ�
*/
void XOR(int *nCodes, const int *nMaskingNo, int nLength)
{
	for (int i = 0; i < nLength; ++i)
	{
		nCodes[i] ^= nMaskingNo[i];
	}
}
