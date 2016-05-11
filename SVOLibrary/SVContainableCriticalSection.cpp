// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVContainableCriticalSection
// * .File Name       : $Workfile:   SVContainableCriticalSection.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:52:20  $
// ******************************************************************************

#include "stdafx.h"
#include "SVContainableCriticalSection.h"

// these functions are in the .CPP instead of the .INL
// because they have WINNT 0x0500 specific function calls.
// This will allow the code to be used in projects where WINNT 0x0500 is not defined.
SVContainableCriticalSection::SVContainableCriticalSection( DWORD p_dwSpinCount /*= 0*/ )
{
	m_plRefCount = new long;
	*m_plRefCount = 1;
	m_pCritSec = new CRITICAL_SECTION;
	m_dwSpinCount = p_dwSpinCount;
	::InitializeCriticalSectionAndSpinCount(m_pCritSec, m_dwSpinCount);
}
