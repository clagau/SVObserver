//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionInitiator
//* .File Name       : $Workfile:   SVFileAcquisitionInitiator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:35:54  $
//******************************************************************************
#ifndef SVFILEACQUISITIONINITIATOR_H
#define SVFILEACQUISITIONINITIATOR_H

#include "SVAcquisitionInitiator.h"

class SVFileAcquisitionInitiator
{
private:
	SVAcquisitionInitiator m_acquisitionInitiator;

public:
	SVFileAcquisitionInitiator();
	SVFileAcquisitionInitiator(SVAcquisitionInitiator& rAcquisitionInitiator);
	~SVFileAcquisitionInitiator();

	void Create( SVAcquisitionInitiator& rFunc );
	void Destroy();

	HRESULT FireAcquisitionTrigger();

	static HRESULT CALLBACK TriggerCallback( void *p_pvOwner, void *p_pvData );
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFileAcquisitionInitiator.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:35:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Oct 2008 16:28:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/