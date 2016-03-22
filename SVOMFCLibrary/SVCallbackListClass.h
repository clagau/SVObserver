// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCallbackListClass
// * .File Name       : $Workfile:   SVCallbackListClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:00:28  $
// ******************************************************************************

#pragma once

#include "SVContainerLibrary/SVList.h"
#include "SVCallbackStruct.h"

class SVCallbackListClass : public SVList< SVCallbackStruct >
{
public:
	SVCallbackListClass();
	virtual ~SVCallbackListClass();

	CString m_csName;

	bool m_bIsStarted;
};

