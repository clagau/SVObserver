// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVODataResponseClass
// * .File Name       : $Workfile:   SVODataResponseClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:06  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "EventTime.h"
#include "TriggerRecordController/IImage.h"
#pragma endregion Includes

/*
This object holds the data response information to a event.
*/
class SVODataResponseClass : public EventTime
{
public:
	SVODataResponseClass() {}
	SVODataResponseClass(const SVODataResponseClass &rRhs) : EventTime(rRhs)
		, m_pImage(rRhs.m_pImage)
	{
	}

	virtual ~SVODataResponseClass() {}

	virtual void reset()
	{ 
		__super::reset();
		m_pImage.reset();
	}

	const SvTrc::IImagePtr& getImage() const { return m_pImage; }
	void setImage(const SvTrc::IImagePtr& pImage) { m_pImage = pImage; }

private:
	SvTrc::IImagePtr m_pImage{nullptr};
};

