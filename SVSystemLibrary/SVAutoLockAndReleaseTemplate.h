//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAutoLockAndReleaseTemplate
//* .File Name       : $Workfile:   SVAutoLockAndReleaseTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:48:54  $
//******************************************************************************

#pragma once

template< typename SVLockType >
class SVAutoLockAndReleaseTemplate
{
public:
	SVAutoLockAndReleaseTemplate();

	virtual ~SVAutoLockAndReleaseTemplate();

	bool Assign( SVLockType* pLock, DWORD WaitTime = INFINITE );

protected:
	SVLockType* m_pLock;

};

#include "SVAutoLockAndReleaseTemplate.inl"


