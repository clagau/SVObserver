// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODigitalStruct
// * .File Name       : $Workfile:   SVIODigitalStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:46:52  $
// ******************************************************************************

#pragma once

struct SVIODigitalStruct  
{
	SVIODigitalStruct() {};
	SVIODigitalStruct(const SVIODigitalStruct& rRhs) {*this = rRhs;};

	const SVIODigitalStruct& operator= (const SVIODigitalStruct& rRhs)
	{
		m_bIsInverted = rRhs.m_bIsInverted;
		m_bIsForced = rRhs.m_bIsForced;
		m_bForcedValue = rRhs.m_bForcedValue;
		m_bValue = rRhs.m_bValue;
		return *this;
	};

	bool m_bIsInverted{true};
	bool m_bIsForced{false};
	bool m_bForcedValue{false};
	bool m_bValue{false};
};

typedef std::vector<SVIODigitalStruct> SVIODigitalStructVector;
