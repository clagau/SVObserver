// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCallbackListArrayClass
// * .File Name       : $Workfile:   SVCallbackListArrayClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:59:40  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCallbackListArrayClass.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVCallbackListArrayClass::SVCallbackListArrayClass()
: SVVector< SVCallbackListClass >()
{
	m_bIsStarted = false;
}

SVCallbackListArrayClass::~SVCallbackListArrayClass()
{
}

