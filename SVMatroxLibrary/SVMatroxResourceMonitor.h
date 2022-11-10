// ******************************************************************************
// * COPYRIGHT (c) 2011 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMatroxIdentifierEnum.h"
#pragma endregion Includes
#ifdef _DEBUG
#define MONITOR_MIL_RESOURCES
#endif

// @TODO - this needs to be in a DLL (not a static lib)
class SVMatroxResourceMonitor
{
public:
	static HRESULT InsertIdentifier( SVMatroxIdentifierEnum p_IdentifierType, __int64 p_Identifier );
	static HRESULT EraseIdentifier( SVMatroxIdentifierEnum p_IdentifierType, __int64 p_Identifier );
	static void OutputDebug();
	

private:
	typedef std::set< __int64 > SVIdentifierSet;
	mutable std::mutex m_Mutex;

	std::vector< SVIdentifierSet > m_Identifiers;
	SVIdentifierSet m_AllIdentifiers;

	SVMatroxResourceMonitor();
	~SVMatroxResourceMonitor();
#ifdef MONITOR_MIL_RESOURCES
	static SVMatroxResourceMonitor& Instance();
	bool FindReference(__int64 p_Identifier) const;
#endif 

	SVMatroxResourceMonitor( const SVMatroxResourceMonitor& p_rObject );
	const SVMatroxResourceMonitor& operator=( const SVMatroxResourceMonitor& p_rObject );
};

/// Destroys a matrox handle with a defined method.
/// \param rId [in] ID of the handle.
/// \param pFreeFunc [in] function pointer to the matrox free function (e.g. MblobFree).
/// \param identifierType [in] Identifer to remove the entry from the debug class SVMatroxResourceMonitor.
/// \returns HRESULT
HRESULT DestroyMatroxId(__int64& rId, void(*pFreeFunc)(MIL_ID), SVMatroxIdentifierEnum identifierType);
