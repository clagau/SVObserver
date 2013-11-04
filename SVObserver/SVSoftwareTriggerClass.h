//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerClass
//* .File Name       : $Workfile:   SVSoftwareTriggerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:08:46  $
//******************************************************************************

#ifndef SVSOFTWARETRIGGERCLASS_H
#define SVSOFTWARETRIGGERCLASS_H

#include "SVTriggerClass.h"
#include "SVAcquisitionInitiator.h"

class SVSoftwareTriggerClass : public SVTriggerClass
{
private:
	SVAcquisitionInitiator m_acquisitionInitiator;

public:
	SVSoftwareTriggerClass(LPCTSTR deviceName);
	virtual ~SVSoftwareTriggerClass();

	virtual HRESULT RegisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller );
	virtual HRESULT UnregisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller );
	
	static HRESULT CALLBACK TriggerCallback( void *p_pvOwner, void *p_pvData );
	static HRESULT CALLBACK TriggerCompleteCallback( void *p_pvOwner, void *p_pvData );

	virtual void RegisterAcquistionInitiator( SVAcquisitionInitiator& rFunc); 

	virtual HRESULT EnableInternalTrigger();
	virtual HRESULT FireAcquisitionTrigger();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVSoftwareTriggerClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:08:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Jan 2013 16:46:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated fucntioality associated with triggers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 12:26:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 11:19:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
