// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIntelRAIDStatusClass
// * .File Name       : $Workfile:   SVOIntelRAIDStatusClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 13:02:38  $
// ******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVString.h"

class SVOIntelRAIDStatusClass  
{
public:
	SVOIntelRAIDStatusClass();
	virtual ~SVOIntelRAIDStatusClass();

	HRESULT UpdateStatus();
	HRESULT CheckStatus();

	const SVString& GetRaidStatus();
	const SVString& GetErrorStatus();

	const HANDLE GetCheckEvent();

protected:
	HANDLE m_hCheckEvent;

	SVString m_RaidStatus;
	SVString m_ErrorStatus;
};

