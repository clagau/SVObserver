// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVQueueObject
// * .File Name       : $Workfile:   SVQueueObject.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:22:52  $
// ******************************************************************************

#include "stdafx.h"
#include "SVQueueObject.h"

bool __stdcall InitializeCriticalSectionAndSpinCount_(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount)
{
	return (TRUE == ::InitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount));
}

