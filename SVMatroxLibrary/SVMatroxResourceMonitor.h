// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxResourceMonitor
// * .File Name       : $Workfile:   SVMatroxResourceMonitor.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Jun 2014 14:14:10  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <set>
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVMatroxIdentifierEnum.h"
#pragma endregion Includes

// @TODO - this needs to be in a DLL (not a static lib)
class SVMatroxResourceMonitor
{
public:
	typedef SVAutoLockAndReleaseTemplate< SVCriticalSection > SVAutoLock;
	static HRESULT InsertIdentifier( SVMatroxIdentifierEnum p_IdentifierType, __int64 p_Identifier );
	static HRESULT EraseIdentifier( SVMatroxIdentifierEnum p_IdentifierType, __int64 p_Identifier );

	static HRESULT GetAutoLock( SVAutoLock& p_rAutoLock );

protected:
	typedef std::set< __int64 > SVIdentifierSet;
	mutable SVCriticalSectionPtr m_CriticalSectionPtr;
	std::vector< SVIdentifierSet > m_Identifiers;
	SVIdentifierSet m_AllIdentifiers;

	SVMatroxResourceMonitor();
	static SVMatroxResourceMonitor& Instance();
	HRESULT ValidateCriticalSection() const;
	bool FindReference(__int64 p_Identifier) const;

private:
	SVMatroxResourceMonitor( const SVMatroxResourceMonitor& p_rObject );
	const SVMatroxResourceMonitor& operator=( const SVMatroxResourceMonitor& p_rObject );
};

