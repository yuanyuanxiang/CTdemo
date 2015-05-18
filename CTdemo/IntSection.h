#pragma once

// ***
// 交线类：记录了射线与像素相交的信息，如像素行列坐标及交线长。
// ***

class CIntSection
{
public:
	float m_xPos;
	float m_yPos;
	float m_Value;

public:
	CIntSection(void);
	CIntSection(int x, int y);
	CIntSection(int x, int y, int v);
	~CIntSection(void);

	friend bool operator == (CIntSection &a, CIntSection &b);
	friend bool operator < (CIntSection &a, CIntSection &b);
	friend bool operator > (CIntSection &a, CIntSection &b);
	friend float Distance(CIntSection &a, CIntSection &b);
};