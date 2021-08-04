//*****************************************************************************
// \copyright COPYRIGHT (c) 2021/08/02,2021/08/02 by Seidenader Maschinenbau GmbH. All Rights Reserved 
// \file LookUp.
//Class for managing a 2dim array using as LUT table for arithmetic image operation
//******************************************************************************

#pragma region Includes 
#include "stdafx.h"
#include "LookUp.h"
#include "SVMatroxLibrary/SVMatroxSimpleEnums.h"
#pragma endregion Includes

namespace
{

template <typename T>
T clip(double val)
{

	if (val <= std::numeric_limits<T>::min())
	{
		return std::numeric_limits<T>::min();
	}
	else if (val >= std::numeric_limits<T>::max())
	{
		return   std::numeric_limits<T>::max();
	}
	return  static_cast<T>(val);
}

class InitFunctions
{
public:
	InitFunctions(double gain, double offset, int depth) : m_gain(gain), m_offset(offset), m_depth(depth)
	{
		;
	};

	BYTE sub(double x, double y)
	{
		double val = (x - y) * m_gain + m_offset;
		return clip<BYTE>(val);
	}
	BYTE subabs(double x, double y)
	{
		double val = abs(x - y) * m_gain + m_offset;
		return clip<BYTE>(val);
	}
	BYTE add(double x, double y)
	{
		double val = (x + y) * m_gain + m_offset;
		return clip<BYTE>(val);
	}
	BYTE mult(double x, double y)
	{
		double val = (x * y) * m_gain + m_offset;
		return clip<BYTE>(val);
	}

	BYTE div(double x, double y)
	{

		if (y == 0)
		{
			y = 0.00001;
		}
		double val = (x / y) * m_gain + m_offset;
		return clip<BYTE>(val);
	}

	// cppcheck-suppress unusedFunction
	BYTE Pythagoras(double x, double y)
	{

		double val = sqrt(x * x + y * y) * m_gain + m_offset;
		return clip<BYTE>(val);
	}

	BYTE Pas(double x, double)
	{

		double val = x * m_gain + m_offset;
		return clip<BYTE>(val);
	}

private:
	double m_gain;
	double m_offset;
	int m_depth;


};
}

LookUp::LookUp(int depth)
{
	m_Data = new BYTE[depth * depth];
	m_depth = depth;

}

LookUp::~LookUp()
{
	delete[] m_Data;
}

void LookUp::Init(double gain, double offset, int op)
{
	if (m_gain == gain && m_offset == offset && m_operator == op)
	{
		return;
	}

	InitFunctions ifuncs(gain, offset, m_depth);
	std::function<BYTE(double x, double y)> ifunc;

	switch (op & ~SVSaturation)
	{
		case    SVImageAdd:
			ifunc = [&ifuncs](double x, double y){return ifuncs.add(x, y); };
			break;
		case	SVImageDiv:
			ifunc = [&ifuncs](double x, double y){return ifuncs.div(x, y); };
			break;
		case	SVImageMult:
			ifunc = [&ifuncs](double x, double y){return ifuncs.mult(x, y); };
			break;
		case	SVImageSub:
			ifunc = [&ifuncs](double x, double y){return ifuncs.sub(x, y); };
			break;
		case	SVImageSubAbs: // M_SUB_ABS 
			ifunc = [&ifuncs](double x, double y){return ifuncs.subabs(x, y); };
			break;
		default:
			ifunc = [&ifuncs](double x, double y){return ifuncs.Pas(x, y); };
			break;
	}

	for (int x = 0; x < m_depth; x++)
	{
		for (int y = 0; y < m_depth; y++)
		{
			m_Data[m_depth * y + x] = ifunc(x, y);
		}
	}
}

BYTE LookUp::get(int x, int y)
{
	return m_Data[m_depth * y + x];
}
