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


/** 放大矩形 */
void ZoomRect(CLogoRect &rect, float rate)
{
	rect.left = int(rate * rect.left);
	rect.top = int(rate * rect.top);
	rect.right = int(rate * rect.right);
	rect.bottom = int(rate * rect.bottom);
}


/** 移动矩形 */
void MoveRect(CLogoRect &rect, float dx, float dy)
{
	rect.left = int(rect.left + dx);
	rect.top = int(rect.top + dy);
	rect.right = int(rect.right + dx);
	rect.bottom = int(rect.bottom + dy);
}


/** 
* @brief 对数据进行掩码操作
* @param[in] *nCodes		待编码数据
* @param[in] *nMaskingNo	掩码数据
* @param[in] nLength		数据长度
* @note XOR可以实现对数据的初级加密。
*/
void XOR(int *nCodes, const int *nMaskingNo, int nLength)
{
	for (int i = 0; i < nLength; ++i)
	{
		nCodes[i] ^= nMaskingNo[i];
	}
}
