// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxResourceMonitor
// * .File Name       : $Workfile:   SVMatroxResourceMonitor.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 11:16:50  $
// ******************************************************************************
#ifndef SVMATROXRESOURCEMONITOR_H
#define SVMATROXRESOURCEMONITOR_H

#include <set>
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVMatroxStatusInformation.h"
#include "SVMatroxIdentifierEnum.h"

// SEJ - this needs to be in a DLL Joe!!! (not a static lib)
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
	
private:
	SVMatroxResourceMonitor( const SVMatroxResourceMonitor& p_rObject );
	const SVMatroxResourceMonitor& operator=( const SVMatroxResourceMonitor& p_rObject );
};
#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxResourceMonitor.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 11:16:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 15:27:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2011 14:29:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
