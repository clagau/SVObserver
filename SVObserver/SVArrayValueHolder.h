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

#ifndef ARRAYVALUEHOLDER_H
#define ARRAYVALUEHOLDER_H

#include <vector>
#include "SVLoki/TypeManip.h"	// Loki

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
};// end class SVArrayValueHolderBase



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
	const type& operator = (const type&);
	virtual bool IsArray() const;

	array_type m_array;
	default_type m_default;
protected:
	virtual SVArrayValueHolderBase* Clone() const;
	virtual const SVArrayValueHolderBase& Assign(const SVArrayValueHolderBase& rhs);
};// end class SVArrayValueHolder 




inline const SVArrayValueHolderBase& SVArrayValueHolderBase::operator = (const SVArrayValueHolderBase& rhs)
{
	Assign(rhs);
	return *this;
}




template <typename T, typename D>
SVArrayValueHolder<T,D>::SVArrayValueHolder(const SVArrayValueHolderBase& rhs_)
{
	ASSERT( typeid(*this) == typeid(rhs) );
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
	ASSERT( typeid(*this) == typeid(rhs_) );
	const SVArrayValueHolder<T,D>& rhs = dynamic_cast < const SVArrayValueHolder<T,D>& > (rhs_);
	*this = rhs;
	return *this;
}

template <typename T, typename D>
bool SVArrayValueHolder<T,D>::IsArray() const
{
	return ! Loki::IsEqualType< value_type, default_type >::value;
}

#endif

