#pragma once

#include <cmath>

/**
* @class CIntSection
* @brief �����ࣺ��¼�������������ཻ����Ϣ���������������꼰���߳���
*/
class CIntSection
{
public:
	float m_xPos;
	float m_yPos;
	float m_Value;

public:
	CIntSection(void) : m_xPos(0), m_yPos(0), m_Value(0){ }

	CIntSection(float x, float y) : m_xPos(x), m_yPos(y), m_Value(0){ }

	CIntSection(float x, float y, float v) : m_xPos(x), m_yPos(y), m_Value(v){ }

	~CIntSection(void){ }

	friend inline bool operator == (const CIntSection &a, const CIntSection &b)
	{
		return (a.m_xPos == b.m_xPos);
	}

	friend inline bool operator < (const CIntSection &a, const CIntSection &b)
	{
		return (a.m_xPos < b.m_xPos);
	}

	friend inline bool operator >(const CIntSection &a, const CIntSection &b)
	{
		return (a.m_xPos > b.m_xPos);
	}

	friend inline float Distance(const CIntSection &a, const CIntSection &b)
	{
		float dx = a.m_xPos - b.m_xPos;
		float dy = a.m_yPos - b.m_yPos;
		return sqrt(dx * dx + dy * dy);
	}
};
