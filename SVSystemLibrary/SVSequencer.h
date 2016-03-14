//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSequencer
//* .File Name       : $Workfile:   SVSequencer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:01:48  $
//******************************************************************************
#ifndef SVSEQUENCER_H
#define SVSEQUENCER_H

#include <iterator>

template<typename IteratorType>
struct SVSequencer
{
	typedef typename std::iterator_traits<IteratorType>::value_type ValueType;

	IteratorType startIt;
	IteratorType endIt;
	IteratorType currentIt;
	bool bContinuousMode;

	SVSequencer();

	void Init(const IteratorType& begin, const IteratorType& end, bool bWrap=true);
	
	ValueType GetNext();
};

#include "SVSequencer.inl"

#endif

