//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSequencer
//* .File Name       : $Workfile:   SVSequencer.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:02:02  $
//******************************************************************************

template<typename IteratorType>
SVSequencer<IteratorType>::SVSequencer(ValueType def)
: bContinuousMode(true),
m_default(def)
{
}

template<typename IteratorType>
void SVSequencer<IteratorType>::Init(const IteratorType& begin, const IteratorType& end, bool bWrap)
{
	bContinuousMode = bWrap;
	startIt = begin;
	endIt = end;
	currentIt = startIt;
}

template<typename IteratorType>
typename SVSequencer<IteratorType>::ValueType SVSequencer<IteratorType>::GetNext()
{
	ValueType info = m_default;
	if (currentIt == endIt)
	{
		// if continuous - wrap back around
		if (bContinuousMode)
		{
			currentIt = startIt;
		}
	}
	if (currentIt != endIt)
	{
		info = (*currentIt);
		currentIt++;
	}
	return info;
}

