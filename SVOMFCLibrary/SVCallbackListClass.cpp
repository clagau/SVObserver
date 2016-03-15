// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCallbackListClass
// * .File Name       : $Workfile:   SVCallbackListClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:00:12  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCallbackListClass.h"

SVCallbackListClass::SVCallbackListClass()
: SVList< SVCallbackStruct >()
{
	m_bIsStarted = false;
}

SVCallbackListClass::~SVCallbackListClass()
{
}

