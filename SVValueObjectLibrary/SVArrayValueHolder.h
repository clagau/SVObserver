//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVArrayValueHolder
//* .File Name       : $Workfile:   SVArrayValueHolder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:33:34  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <vector>
#include "SVLoki/TypeManip.h"	// Loki
#pragma endregion Includes

class SVArrayValueHolderBase
{
public:
	SVArrayValueHolderBase() {};
	virtual ~SVArrayValueHolderBase() {};
	const SVArrayValueHolderBase& operator = (const SVArrayValueHolderBase& rhs);
	virtual bool IsArray() const = 0;
protected:
	virtual SVArrayValueHolderBase* Clone() const = 0;
	virtual const SVArrayValueHolderBase& Assign(const SVArrayValueHolderBase& rhs) = 0;
};

template <typename T, typename D=T>
class SVArrayValueHolder : public SVArrayValueHolderBase
{
public:
	typedef SVArrayValueHolder<T,D> type;
	typedef T value_type;
	typedef D default_type;
	typedef std::vector< T > array_type;

	SVArrayValueHolder<T,D>() {}
	SVArrayValueHolder<T,D>(const SVArrayValueHolderBase& rhs);
	virtual ~SVArrayValueHolder<T,D>() {}
	const type& operator = (const type&);
	virtual bool IsArray() const override;

	array_type m_array;
	default_type m_default;
protected:
	virtual SVArrayValueHolderBase* Clone() const override;
	virtual const SVArrayValueHolderBase& Assign(const SVArrayValueHolderBase& rhs) override;
};

inline const SVArrayValueHolderBase& SVArrayValueHolderBase::operator = (const SVArrayValueHolderBase& rhs)
{
	Assign(rhs);
	return *this;
}

template <typename T, typename D>
SVArrayValueHolder<T,D>::SVArrayValueHolder(const SVArrayValueHolderBase& rhs_)
{
	assert( typeid(*this) == typeid(rhs) );
	const SVArrayValueHolder<T,D>& rhs = dynamic_cast < const SVArrayValueHolder<T,D>& > (rhs_);
	*this = rhs;
}

template <typename T, typename D>
const SVArrayValueHolder<T,D>& SVArrayValueHolder<T,D>::operator = (const SVArrayValueHolder<T,D>& rhs)
{
	m_array = rhs.m_array;
	return *this;
}

template <typename T, typename D>
SVArrayValueHolderBase* SVArrayValueHolder<T,D>::Clone() const
{
	return new SVArrayValueHolder<T,D>(*this);
}

template <typename T, typename D>
const SVArrayValueHolderBase& SVArrayValueHolder<T,D>::Assign(const SVArrayValueHolderBase& rhs_)
{
	assert( typeid(*this) == typeid(rhs_) );
	const SVArrayValueHolder<T,D>& rhs = dynamic_cast < const SVArrayValueHolder<T,D>& > (rhs_);
	*this = rhs;
	return *this;
}

template <typename T, typename D>
bool SVArrayValueHolder<T,D>::IsArray() const
{
	return ! Loki::IsEqualType< value_type, default_type >::value;
}

