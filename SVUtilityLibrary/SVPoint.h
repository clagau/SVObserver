//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file SVPoint.h
//*****************************************************************************
// Class for x,y coordinates
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <type_traits>
#include "StringHelper.h"
#pragma endregion Includes


template <typename T>
class SVPoint
{
public:
	//@TODO[gra][8.10][19.11.2018]: The format for integral types should be "( %d, %d )" (one extra space)
	//however the script tests need to be adapted otherwise errors will occur
	static constexpr auto cFormat = std::is_integral<T>::value ? "( %d, %d)" : "( %lf, %lf )";
	static_assert(std::is_arithmetic<T>::value, "Must be a number!");

	SVPoint()
	{ 
	}
	explicit SVPoint( const POINT& rRhs ) { *this = rRhs; }
	SVPoint(const SVPoint& rRhs) = default;
	SVPoint(SVPoint&& Rhs) = default;

	SVPoint( T x, T y ){ m_x = x; m_y = y; }
	explicit SVPoint(  const _variant_t& rVariant ) { *this = rVariant; }

	virtual ~SVPoint() = default;

	std::string toString() const
	{
		return SvUl::Format(cFormat, m_x, m_y);
	}

	SVPoint& operator=(const POINT &rRhs)
	{
		m_x = static_cast<T>(rRhs.x);
		m_y = static_cast<T>(rRhs.y);

		return (*this);
	}

	SVPoint& operator=(const SVPoint &rRhs) = default;

	SVPoint& operator=( const _variant_t& rRhs)
	{
		std::string temp = SvUl::createStdString(rRhs);

		double x;
		double y;
		if (2 == _stscanf(temp.c_str(), cFormat, &x, &y))
		{
			m_x = static_cast<T> (x);
			m_y = static_cast<T> (y);
		}
		else
		{
			m_x = {};
			m_y = {};
		}

		return (*this);
	}

	bool operator==(const SVPoint &rRhs) const
	{
		return (m_x == rRhs.m_x && m_y == rRhs.m_y);
	}

	bool operator!=(const SVPoint &rRhs) const
	{
		return (m_x != rRhs.m_x || m_y != rRhs.m_y);
	}

	friend SVPoint operator-(const SVPoint &rLhs, const SVPoint &rRhs)
	{
		SVPoint point {rLhs};
		point.m_x -= rRhs.m_x;
		point.m_y -= rRhs.m_y;
		return point;
	}

	friend SVPoint operator+(const SVPoint &rLhs, const SVPoint &rRhs)
	{
		SVPoint point {rLhs};
		point.m_x += rRhs.m_x;
		point.m_y += rRhs.m_y;
		return point;
	}

	const SVPoint& operator-=(const SVPoint &rRhs)
	{
		*this = *this - rRhs;

		return *this;
	}

	const SVPoint& operator+=(const SVPoint &rRhs)
	{
		*this = *this + rRhs;
		return *this;
	}

	friend SVPoint operator/(const SVPoint &rPoint, double constant)
	{
		return SVPoint{static_cast<T> (rPoint.m_x / constant), static_cast<T> (rPoint.m_y / constant)};
	}

	explicit operator POINT() const
	{
		return POINT {static_cast<long> (m_x), static_cast<long> (m_y)};
	}

	explicit operator _variant_t () const
	{
		return _variant_t.SetString(toString());
	}

	void clear()
	{
		m_x = {};
		m_y = {};
	}

	POINT getPoint() const { return static_cast<POINT> (*this); }

	T m_x {};
	T m_y {};
};

