//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIMTypeInfoStruct
//* .File Name       : $Workfile:   SVIMTypeInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:58:58  $
//******************************************************************************

#pragma once

struct SVIMTypeInfoStruct
{
	bool m_Supported;
	short m_MaxDigitizers;
	short m_MaxTriggers;

	SVIMTypeInfoStruct( bool p_Supported, short p_MaxDigitizers, short p_MaxTriggers ) :
		m_Supported( p_Supported )
	, m_MaxDigitizers( p_MaxDigitizers )
	, m_MaxTriggers( p_MaxTriggers)
	{
	}
};

