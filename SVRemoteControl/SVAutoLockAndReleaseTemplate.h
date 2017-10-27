//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAutoLockAndReleaseTemplate
//* .File Name       : $Workfile:   SVAutoLockAndReleaseTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:37:36  $
//******************************************************************************

#pragma once

template< typename SVLockType >
class SVAutoLockAndReleaseTemplate
{
public:
	SVAutoLockAndReleaseTemplate();

	virtual ~SVAutoLockAndReleaseTemplate();

	BOOL Assign( SVLockType* p_pLock, DWORD p_WaitTime = INFINITE );

protected:
	SVLockType* m_pLock;
};

#include "SVAutoLockAndReleaseTemplate.inl"
