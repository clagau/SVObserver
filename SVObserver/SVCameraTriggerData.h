//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraTriggerData
//* .File Name       : $Workfile:   SVCameraTriggerData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:52:48  $
//******************************************************************************
#ifndef INCL_SVCAMERATRIGGERDATA_H
#define INCL_SVCAMERATRIGGERDATA_H

#include "SVMaterialsLibrary/SVMaterials.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVDoubleValueObjectClass.h"
#include "SVBoolValueObjectClass.h"
#include "SVIOEntryHostStruct.h"

class SVCameraTriggerData : public SVObjectClass 
{
private:
	SVDoubleValueObjectClass m_timestamp;
	SVBoolValueObjectClass m_lineInState;

public:
	SVCameraTriggerData();
	virtual ~SVCameraTriggerData();

	SVIOEntryHostStructPtr SetupLineStateInput(long objectDepth);
	SVIOEntryHostStructPtr SetupTimestampInput(long objectDepth);

	SVString GetTimestampName() const;
	SVString GetLineStateName() const;

	void Set(long index, const SVMaterials& rMaterials);

private:
	void init();
	void destroy();
	void SetLineState(long index, double timeStamp, bool state);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCameraTriggerData.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:52:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Jan 2013 10:59:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
