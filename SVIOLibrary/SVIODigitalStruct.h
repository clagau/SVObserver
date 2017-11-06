// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
	SVIODigitalStruct(const SVIODigitalStruct& rOriginal) {*this = rOriginal;};

	const SVIODigitalStruct& operator= (const SVIODigitalStruct& rOriginal)
	{
		m_bIsInverted = rOriginal.m_bIsInverted;
		m_bIsForced = rOriginal.m_bIsForced;
		m_bForcedValue = rOriginal.m_bForcedValue;
		m_bValue = rOriginal.m_bValue;
		return *this;
	};

	bool m_bIsInverted = false;
	bool m_bIsForced = false;
	bool m_bForcedValue = false;
	bool m_bValue = false;
};

typedef std::vector<SVIODigitalStruct> SVIODigitalStructVector;
