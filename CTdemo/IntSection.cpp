#include "stdafx.h"
#include "IntSection.h"
#include "cmath"

CIntSection::CIntSection(void)
{
	m_xPos = m_yPos = m_Value = 0.f;
}


CIntSection::CIntSection(int x, int y)
{
	m_xPos = x;
	m_yPos = y;
	m_Value = 0.f;
}


CIntSection::CIntSection(int x, int y, int v)
{
	m_xPos = x;
	m_yPos = y;
	m_Value = v;
}


CIntSection::~CIntSection(void)
{
}


bool operator == (CIntSection &a, CIntSection &b)
{
	return (a.m_xPos == b.m_xPos);
}


bool operator < (CIntSection &a, CIntSection &b)
{
	return (a.m_xPos < b.m_xPos);
}


bool operator > (CIntSection &a, CIntSection &b)
{
	return (a.m_xPos > b.m_xPos);
}


// 计算两点之间的距离
float Distance(CIntSection &a, CIntSection &b)
{
	float dx = a.m_xPos - b.m_xPos;
	float dy = a.m_yPos - b.m_yPos;
	return sqrt(dx * dx + dy * dy);
}