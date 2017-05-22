#pragma once

/** 
* @file DataTypes.h
* @brief ���ĵ������˳�������õ�����������
* @note �����Ҫ��ANDROIDʹ�ã���#define ANDROID_CPP
* @date 2016/8/24 ע
*/

#include <math.h>
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned char uchar;
typedef BYTE *LPBYTE;
typedef unsigned int uint;
#define MAX_MODULESIZE		177		// QR�����ߴ�
typedef BYTE qrMat[MAX_MODULESIZE];	// QR����ؾ���

#if defined(ANDROID_CPP)

#include <stdio.h>

#ifndef NO_ICONV
#define NO_ICONV
#endif

#ifndef ASSERT 
#include <assert.h>
#define ASSERT assert
#endif

#endif

#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL 0
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))					/**< ���ֵ */
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))					/**< ��Сֵ */
#endif

#define MAX_CODEUTF8NUM		1024										/**< ������utf8�������� */

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)					/**< ����ÿ���ֽ���(4�ֽڶ���) */

#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }/**< ��ȫɾ��ָ��p  */
#endif

#ifndef RGB
/// RGB
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif

#ifndef ZeroMemory
/// ����
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#endif

#define PI 3.141592653f /**< PI */
#define QR_MARGIN	4	/**< QR�߽� */

#ifdef ANDROID_CPP		/**< ����ڰ�װ�±��룬��Ҫ����ĳЩ�ļ� */

#include "stdlib.h"
#include "string.h"

#define TRACE printf

typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned long		COLORREF;
typedef unsigned long		ULONG_PTR;
typedef ULONG_PTR			DWORD_PTR;
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define GetRValue(rgb)      (LOBYTE(rgb))
#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))

#endif // ANDROID_CPP

/** 
* @class float2
* @brief ��ά��������(x, y)
*/
class float2
{
public:
	float x, y;
public:
	float2() : x(0), y(0)
	{
	}
	float2(float a) : x(a), y(a)
	{
	}
	float2(float a, float b) : x(a), y(b)
	{
	}
	inline operator float() { return sqrt(x * x + y * y); }
	// =
	float2 operator = (const float2& point)
	{
		x = point.x;
		y = point.y;
		return *this;
	}
	float2 operator = (const float val)
	{
		x = val;
		y = val;
		return *this;
	}
	// -
	float2 operator - ()
	{
		x = -x;
		y = -y;
		return *this;
	}
	// ==
	bool operator == (const float2 & point)
	{
		return (x == point.x && y == point.y);
	}
	bool operator == (const float val)
	{
		return (x == val && y == val);
	}
	// !=
	bool operator != (const float2 & point)
	{
		return (x != point.x || y != point.y);
	}
	bool operator != (const float val)
	{
		return (x != val || y != val);
	}
	// += -= *= /=
	float2 operator += (const float2 & pos)
	{
		x += pos.x;
		y += pos.y;
		return *this;
	}
	float2 operator -= (const float2 & pos)
	{
		x -= pos.x;
		y -= pos.y;
		return *this;
	}
	float2 operator *= (const float2 & pos)
	{
		x *= pos.x;
		y *= pos.y;
		return *this;
	}
	float2 operator /= (const float2 & pos)
	{
		x /= pos.x;
		y /= pos.y;
		return *this;
	}
	float2 operator += (const float val)
	{
		x += val;
		y += val;
		return *this;
	}
	float2 operator -= (const float val)
	{
		x -= val;
		y -= val;
		return *this;
	}
	float2 operator *= (const float val)
	{
		x *= val;
		y *= val;
		return *this;
	}
	float2 operator /= (const float val)
	{
		x /= val;
		y /= val;
		return *this;
	}
	// ��������(1)
	friend float2 operator + (const float2 & pos, const float val)
	{
		return float2(pos.x + val, pos.y + val);
	}
	friend float2 operator - (const float2 & pos, const float val)
	{
		return float2(pos.x - val, pos.y - val);
	}
	friend float2 operator * (const float2 & pos, const float val)
	{
		return float2(pos.x * val, pos.y * val);
	}
	friend float2 operator / (const float2 & pos, const float val)
	{
		return float2(pos.x / val, pos.y / val);
	}
	// ��������(2)
	friend float2 operator + (const float val, const float2 & pos)
	{
		return float2(val + pos.x, val + pos.y);
	}
	friend float2 operator - (const float val, const float2 & pos)
	{
		return float2(val - pos.x, val - pos.y);
	}
	friend float2 operator * (const float val, const float2 & pos)
	{
		return float2(val * pos.x, val * pos.y);
	}
	friend float2 operator / (const float val, const float2 & pos)
	{
		return float2(val / pos.x, val / pos.y);
	}
	// ��������
	friend float2 operator + (const float2 & pos1, const float2 & pos2)
	{
		return float2(pos1.x + pos2.x, pos1.y + pos2.y);
	}
	friend float2 operator - (const float2 & pos1, const float2 & pos2)
	{
		return float2(pos1.x - pos2.x, pos1.y - pos2.y);
	}
	friend float2 operator * (const float2 & pos1, const float2 & pos2)
	{
		return float2(pos1.x * pos2.x, pos1.y * pos2.y);
	}
	friend float2 operator / (const float2 & pos1, const float2 & pos2)
	{
		return float2(pos1.x / pos2.x, pos1.y / pos2.y);
	}
	friend float2 abs(const float2 & point)
	{
		return float2(abs(point.x), abs(point.y));
	}
};


/** 
* @class float3
* @brief ��ά��������(x, y. z)
*/
class float3
{
public:
	float x, y, z;
public:
	float3() : x(0), y(0), z(0)
	{
	}
	float3(float a, float b, float c) : x(a), y(b), z(c)
	{
	}
	float3(const float3 & other) : x(other.x), y(other.y), z(other.z)
	{
	}
	float3(COLORREF ref) : x(GetRValue(ref)), y(GetGValue(ref)), z(GetBValue(ref))
	{
	}
	// ת��Ϊ�Ҷ�ֵ
	inline float ToGray() { return 0.299f * x + 0.587f * y + 0.114f * z; }
	// ǿ��ת������,ͬToGray
	inline operator float() { return 0.299f * x + 0.587f * y + 0.114f * z; }
	// =
	float3 operator = (const float3& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
	float3 operator = (const float val)
	{
		x = val;
		y = val;
		z = val;
		return *this;
	}
	// -
	float3 operator - ()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}
	// ==
	bool operator == (const float3& other)
	{
		return (x == other.x && y == other.y && z == other.z);
	}
	bool operator == (const float val)
	{
		return (x == val && y == val && z == val);
	}
	// !=
	bool operator != (const float3& other)
	{
		return (x != other.x || y != other.y || z != other.z);
	}
	bool operator != (const float val)
	{
		return (x != val || y != val || z != val);
	}
	// += -= *= /=
	float3 operator += (const float3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	float3 operator -= (const float3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	float3 operator *= (const float3& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}
	float3 operator /= (const float3& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}
	float3 operator += (const float other)
	{
		x += other;
		y += other;
		z += other;
		return *this;
	}
	float3 operator -= (const float other)
	{
		x -= other;
		y -= other;
		z -= other;
		return *this;
	}
	float3 operator *= (const float other)
	{
		x *= other;
		y *= other;
		z *= other;
		return *this;
	}
	float3 operator /= (const float other)
	{
		x /= other;
		y /= other;
		z /= other;
		return *this;
	}
	// �븡������(1)
	friend float3 operator + (const float3 & point, float val)
	{
		return float3(point.x + val, point.y + val, point.z + val);
	}
	friend float3 operator - (const float3 & point, float val)
	{
		return float3(point.x - val, point.y - val, point.z - val);
	}
	friend float3 operator * (const float3 & point, float val)
	{
		return float3(point.x * val, point.y * val, point.z * val);
	}
	friend float3 operator / (const float3 & point, float val)
	{
		return float3(point.x / val, point.y / val, point.z / val);
	}
	// �븡������(2)
	friend float3 operator + (float val, const float3 & point)
	{
		return float3(val + point.x, val + point.y, val + point.z);
	}
	friend float3 operator - (float val, const float3 & point)
	{
		return float3(val - point.x, val - point.y, val - point.z);
	}
	friend float3 operator * (float val, const float3 & point)
	{
		return float3(val * point.x, val * point.y, val * point.z);
	}
	friend float3 operator / (float val, const float3 & point)
	{
		return float3(val / point.x, val / point.y, val / point.z);
	}
	// ��������
	friend float3 operator + (const float3 & P, const float3 & Q)
	{
		return float3(P.x + Q.x, P.y + Q.y, P.z + Q.z);
	}
	friend float3 operator - (const float3 & P, const float3 & Q)
	{
		return float3(P.x - Q.x, P.y - Q.y, P.z - Q.z);
	}
	friend float3 operator * (const float3 & P, const float3 & Q)
	{
		return float3(P.x * Q.x, P.y * Q.y, P.z * Q.z);
	}
	friend float3 operator / (const float3 & P, const float3 & Q)
	{
		return float3(P.x / Q.x, P.y / Q.y, P.z / Q.z);
	}
	friend float3 abs(const float3 & point)
	{
		return float3(abs(point.x), abs(point.y), abs(point.z));
	}
};

/// RGBת�Ҷ�(@see http://blog.csdn.net/cool1949/article/details/6649429)
#define RGB2GRAY(R, G, B) (  ((R)*38 + (G)*75 + (B)*15) >> 7 )

/// RGB�꣺COLORREFתΪ�Ҷ�ֵ
#define RgbColorRef2Gray(ref)			( (GetRValue(ref)*38 + GetGValue(ref)*75 + GetBValue(ref)*15) >> 7 )

#define BIT_SET(a, b) ((a) |= (1<<(b)))			/**< ����a��bλΪ1 */
#define BIT_CLEAR(a, b) ((a) &= ~(1<<(b)))		/**< ����a��bλΪ0 */
#define BIT_FLIP(a, b) ((a) ^= (1<<(b)))		/**< ����a��bλ */
#define BIT_CHECK(a, b) ((a) & (1<<(b)))		/**< ���a��bλ */

/** 
* @brief ������������
* @note ��������ٶ���
* @see http://blog.csdn.net/anchor89/article/details/5979290
*/
template <class T> inline void SWAP(T & a, T & b)
{
	T c(a); a = b; b = c;
}

/// ���a��iλΪ1��������b��jλΪ1
#define BIT_CHECK_SET(a, i, b, j) if(BIT_CHECK(a, i)) BIT_SET(b, j)

/// �������b��jλΪ1��ʹ1�Ը���p����
#define BIT_RANDOM_SET(p, b, j) if(rand() <= p * RAND_MAX) BIT_SET(b, j)

/** 
* @class CMyRect
* @brief ������
*/
class CMyRect
{
public:
	CMyRect() : left(0), top(0), right(0), bottom(0)
	{
	}
	CMyRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b)
	{
	}
	CMyRect(int nC) : left(nC), top(nC), right(nC), bottom(nC)
	{
	}
	~CMyRect()
	{
	}
	inline int Width() const 
	{
		return right - left;
	}
	inline int Height() const 
	{
		return bottom - top;
	}
	bool operator == (const CMyRect & rect) const 
	{
		return (left == rect.left && right == rect.right
			&& top == rect.top && bottom == rect.bottom);
	}
	inline bool IsEmpty() const 
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0);
	}

public:
	int left, top, right, bottom;
};

/// ͼ�����
typedef CMyRect CLogoRect;

/// ����Ȥ����
typedef CMyRect RoiRect;

/** 
* @class CPixelPoint
* @brief ����
*/
class CPixelPoint
{
public:
	CPixelPoint() : x(0), y(0)
	{
	}
	CPixelPoint(int xPos, int yPos) : x(xPos), y(yPos)
	{
	}
	~CPixelPoint()
	{
	}
	// ����x����Ƚ��������С(P1>P2)
	friend bool operator > (const CPixelPoint &P1, const CPixelPoint &P2)
	{
		return (P1.x > P2.x);
	}
public:
	int x, y;
};

/** 
* @class rgb
* @brief ��ɫ(r, g, b)
*/
template <typename Type>
class rgb
{
public:
	Type r, g, b, a; // a������ʹ4�ֽڶ���
	rgb() : r(0), g(0), b(0), a(0), v(0)
	{
	}
	rgb(Type R, Type G, Type B) : r(R), g(G), b(B), a(0)
	{
		v = ToInt();
	}
	rgb(const rgb & in) : r(in.r), g(in.g), b(in.b)
	{
		v = ToInt();
	}
	// ��ʼ����С����ֵ(3���˷�)
	inline void Init() { v = ToInt(); }
	// ��������rgb����
	inline void Swap(rgb<Type> & other) 
	{
		SWAP(r, other.r);
		SWAP(g, other.g);
		SWAP(b, other.b);
		SWAP(v, other.v);
	}
	// �Ƚ�����rgb���صĴ�С
	bool operator < (const rgb<Type> & other)
	{
		return v < other.v;
	}
private:
	int v; // ���ڶ�����С��ֵ
	// ת��ΪInt���ڱȽϴ�С(3���˷�)
	inline int ToInt() const { return 299 * BYTE(r) + 587 * BYTE(g) + 114 * BYTE(b); }

	//////////////////////////////////////////////////////////////////////////
	// friend
	// ��rgb�ṹ���в�������
	friend void Sort(rgb<Type> *pArray, int Num)
	{
		for (int i = 1; i < Num; ++i)
		{
			for (int j = i; j > 0 && pArray[j] < pArray[j-1]; --j)
			{
				pArray[j].Swap(pArray[j-1]);
			}
		}
	}
};


/**
* @struct BarCodeInfo
* @brief ��ά����Ϣ�ṹ��(6����Ա)
* details ����ȼ�,��ά��汾,����汾,�ַ����ַ�����,ÿģ���С.
*/
struct BarCodeInfo
{
	int m_nEcLevel;				/**< ����ȼ� */
	int m_nVersion;				/**< ��ά��汾 */
	int m_nMaskingNo;			/**< ����汾 */
	int m_nStrLen;				/**< �ַ����� */
	float m_fModuleSize;		/**< ÿģ���С */
	const char* m_pData;		/**< ��ά������ */

	BarCodeInfo()
	{
		memset(this, 0, sizeof(BarCodeInfo));
	}
};


/** 
* @brief �ж�����(i, j)���ھ�����
* @see http://www.cnblogs.com/archimedes/p/writing-efficient-c-and-code-optimization.html
*/
inline BOOL IndexNotInRect(int i, int j, const CLogoRect &rect)
{
	return ( (unsigned(i - rect.top) < (unsigned)rect.Height()) 
		&&   (unsigned(j - rect.left) < (unsigned)rect.Width()) ) ? FALSE : TRUE;
}


/// �����ľ���
inline float Distance(const float2 & P, const float2 & Q)
{
	float dx = (P.x - Q.x);
	float dy = (P.y - Q.y);
	return sqrtf(dx * dx + dy * dy);
}


/// �����ľ���
inline float Distance(const float3 & P, const float3 & Q)
{
	float dx = (P.x - Q.x);
	float dy = (P.y - Q.y);
	float dz = (P.z - Q.z);
	return sqrtf(dx * dx + dy * dy + dz * dz);
}


/// �����ľ���ƽ��
inline float _Distance(const float2 & P, const float2 & Q)
{
	float dx = (P.x - Q.x);
	float dy = (P.y - Q.y);
	return dx * dx + dy * dy;
}


/// �����ľ���ƽ��
inline float _Distance(const float3 & P, const float3 & Q)
{
	float dx = (P.x - Q.x);
	float dy = (P.y - Q.y);
	float dz = (P.z - Q.z);
	return dx * dx + dy * dy + dz * dz;
}

inline float _Distance(const float & P, const float & Q)
{
	return abs(P - Q);
}

/// ģ
inline float fabs(const float2 & pos)
{
	return sqrt(pos.x * pos.x + pos.y * pos.y);
}

/// ģ
inline float fabs(const float3 & pos)
{
	return sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
}

// �����ΰ������Ŵ�
void ZoomRect(CLogoRect &rect, float rate = 1.f);

// ������(dx, dy)�ƶ�����
void MoveRect(CLogoRect &rect, float dx = 0.f, float dy = 0.f);

// �����ݽ����������
void XOR(int *nCodes, const int *nMaskingNo, int nLength);

#define INV_SIGN_BIT	0x7fffffff // ������ת����λ

#define USE_ASM			0			// �Ƿ�ʹ�û�����

// float���������ֵ
inline float FastAbs(float fNum)
{
#if USE_ASM
	float fOut;
	__asm
	{
		MOV EAX, fNum;
		AND EAX, INV_SIGN_BIT;
		MOV fOut, EAX;
	}
	return fOut;
#else
	int *temp = (int*)&fNum;
	int out = *temp & INV_SIGN_BIT;
	return *((float*)&out);
#endif
}

// int���������ֵ
inline int FastAbs(int iNum)
{
#if USE_ASM
	int iOut = 0;
	__asm  
	{  
		MOV EAX, iNum;
		MOV EDX, EAX;
		SAR EDX, 31;
		XOR EAX, EDX;
		SUB EAX, EDX;
		MOV iOut, EAX;
	}  
#else
	int iOut = iNum;
	int temp = iNum;
	temp >>= 31;
	iOut ^= temp;
	iOut -= temp;
#endif
	return iOut;
}