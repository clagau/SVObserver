//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSequencer
//* .File Name       : $Workfile:   SVSequencer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:01:48  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <iterator>

template<typename IteratorType>
struct SVSequencer
{

	using  ValueType = typename std::iterator_traits<IteratorType>::value_type;
	IteratorType startIt;
	IteratorType endIt;
	IteratorType currentIt;
	bool bContinuousMode;

	SVSequencer(ValueType defaultValue);

	void Init(const IteratorType& begin, const IteratorType& end, bool bWrap=true);
	
	ValueType GetNext();
	ValueType m_default;
};

#include "SVSequencer.inl"

