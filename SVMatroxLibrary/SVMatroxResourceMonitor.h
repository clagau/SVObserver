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

//Moved to precompiled header: #include <set>
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVMatroxStatusInformation.h"
#include "SVMatroxIdentifierEnum.h"

// @TODO - this needs to be in a DLL (not a static lib)
class SVMatroxResourceMonitor
{
public:
	typedef SVMatroxStatusInformation::SVStatusCode SVStatusCode;
	typedef SVAutoLockAndReleaseTemplate< SVCriticalSection > SVAutoLock;
	static HRESULT InsertIdentifier( SVMatroxIdentifierEnum p_IdentifierType, SVMatroxIdentifier p_Identifier );
	static HRESULT EraseIdentifier( SVMatroxIdentifierEnum p_IdentifierType, SVMatroxIdentifier p_Identifier );

	static SVStatusCode GetAutoLock( SVAutoLock& p_rAutoLock );

protected:
	typedef std::set< SVMatroxIdentifier > SVIdentifierSet;
	mutable SVCriticalSectionPtr m_CriticalSectionPtr;
	std::vector< SVIdentifierSet > m_Identifiers;
	SVIdentifierSet m_AllIdentifiers;

	SVMatroxResourceMonitor();
	static SVMatroxResourceMonitor& Instance();
	HRESULT ValidateCriticalSection() const;
	bool FindReference(SVMatroxIdentifier p_Identifier) const;

private:
	SVMatroxResourceMonitor( const SVMatroxResourceMonitor& p_rObject );
	const SVMatroxResourceMonitor& operator=( const SVMatroxResourceMonitor& p_rObject );
};

