//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystemInterface
//* .File Name       : $Workfile:   SVMatroxSystemInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:15:32  $
//******************************************************************************

#ifndef SVMATROXSYSTEMINTERFACE_H
#define SVMATROXSYSTEMINTERFACE_H

#include "SVMatroxLibrary/SVMatroxStatusInformation.h"
#include "SVMatroxLibrary/SVMatroxSystem.h"
#include "SVMatroxLibrary/SVMatroxSystemEnums.h"

class SVMatroxSystemInterface
{
public:
	typedef SVMatroxStatusInformation::SVStatusCode SVStatusCode;

	static SVStatusCode Allocate(const SVMatroxString& SystemDescriptor, SVMatroxInt SystemNum, SVMatroxSystem& system);
	static SVStatusCode Release(SVMatroxSystem& SystemId);

	static SVStatusCode Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, long& value);
	static SVStatusCode Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, double& value);
	static SVStatusCode Get(const SVMatroxSystem& SystemId, SVMatroxSystemInquire::SVMatroxSystemInquireEnum InquireType, SVMatroxString& value);

	static SVStatusCode Set(const SVMatroxSystem& SystemId, SVMatroxSystemControl::SVMatroxSystemControlEnum ControlType, long ControlValue);

	static SVStatusCode Destroy(SVMatroxSystem& p_rSystem);

	static SVStatusCode SetHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr);
	static SVStatusCode ReleaseHookFunction(const SVMatroxSystem& SystemId, SVMatroxSystemHook::SVMatroxSystemHookEnum HookType, SVMatroxHookFunctionPtr HookHandlerPtr, void* UserDataPtr);

	static SVStatusCode GetHookInfo(const SVMatroxSystem& SystemId, SVMatroxIdentifier p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, long& value);
	static SVStatusCode GetHookInfo(const SVMatroxSystem& SystemId, SVMatroxIdentifier p_EventId, SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnum HookInfoType, unsigned __int64& value);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxSystemInterface.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 11:15:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 15:29:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Jan 2013 15:41:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised ReleaseHookFunction method to pass the pointer used when acquiring the hook
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jun 2012 14:06:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code needed to implement MIL 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:35:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/