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
//Moved to precompiled header: #include <boost/any.hpp>
#include "EventTime.h"
#pragma endregion Includes

class SVOResponseClass : public EventTime
{
public:
	SVOResponseClass() {}
	SVOResponseClass(const SVOResponseClass &rRhs) : EventTime(rRhs)
		, m_ExtraData {rRhs.m_ExtraData}
	{
	}

	const SVOResponseClass& operator=(const SVOResponseClass &rRhs)
	{
		__super::operator =(rRhs);
		m_ExtraData = rRhs.m_ExtraData;
		return *this;
	}

	virtual ~SVOResponseClass() {}


	virtual void reset()
	{
		__super::reset();
		m_ExtraData.swap(boost::any());
	}

	const boost::any& getExtraData() {return m_ExtraData;}
	void setExtraData( const boost::any& rExtraData ) {m_ExtraData = rExtraData;}

private:
	//This attribute holds context data of the response. 
	boost::any m_ExtraData;  //used by SVCameraTriggerClass::TriggerCallback() only
};

