// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOResponseClass
// * .File Name       : $Workfile:   SVOResponseClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:20  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "EventTime.h"
#pragma endregion Includes

class SVOResponseClass : public EventTime
{
	typedef  std::map<int, _variant_t> IntVariantMap;
public:
	SVOResponseClass() {}
	SVOResponseClass(const SVOResponseClass &rRhs) : EventTime(rRhs)
		, m_Data {rRhs.m_Data}
	{
	}

	SVOResponseClass& operator=(const SVOResponseClass &rRhs) = default;

	virtual ~SVOResponseClass() = default;


	virtual void reset()
	{
		__super::reset();
		m_Data.clear();
	}

	const IntVariantMap& getData() {return m_Data;}
	void setData( const IntVariantMap& rData ) {m_Data = rData;}

private:
	//This attribute holds context data of the response. 
	IntVariantMap m_Data;
};

