//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerRelayClass
//* .File Name       : $Workfile:   SVTriggerRelayClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:51:54  $
//******************************************************************************

#ifndef SVTRIGGERRELAYCLASS_H
#define SVTRIGGERRELAYCLASS_H

#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"

template<typename TriggerHandler>
class SVTriggerRelayClass
{
private:
	SVIOTriggerLoadLibraryClass* m_pTriggerDLL;
	unsigned long m_ulTriggerHandle;
	TriggerHandler m_triggerHandler;
	
public:
	SVTriggerRelayClass(); 
	~SVTriggerRelayClass();
	
	HRESULT RegisterTriggerRelay(SVIOTriggerLoadLibraryClass* pTriggerDLL, unsigned long ulHandle, TriggerHandler& rTriggerHandler);
	HRESULT UnregisterTriggerRelay();
};

#include "SVTriggerRelayClass.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTriggerRelayClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:51:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 13:22:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Oct 2008 16:28:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
