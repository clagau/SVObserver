// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCallbackListArrayClass
// * .File Name       : $Workfile:   SVCallbackListArrayClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:59:58  $
// ******************************************************************************

#ifndef SVCALLBACKLISTARRAYCLASS_H
#define SVCALLBACKLISTARRAYCLASS_H

#include "SVContainerLibrary/SVVector.h"
#include "SVCallbackListClass.h"

class SVCallbackListArrayClass : public SVVector< SVCallbackListClass >
{
public:
	SVCallbackListArrayClass();
	virtual ~SVCallbackListArrayClass();

	bool m_bIsStarted;
};

#endif

