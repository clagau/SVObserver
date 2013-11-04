// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCICallbackStruct
// * .File Name       : $Workfile:   SVCICallbackStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:04  $
// ******************************************************************************

#ifndef SVCICALLBACKSTRUCT_H
#define SVCICALLBACKSTRUCT_H

#include "SVContainerLibrary/SVVector.h"

/*
This class is used to manage callback information for each buffer.
*/
struct SVCICallbackStruct  
{
public:
	SVCICallbackStruct();
	virtual ~SVCICallbackStruct();

	//This attribute holds the index to the associated digitizer.
	unsigned long m_ulDigitizerIndex;
	//This attribute holds the index to the associated buffer.
	unsigned long m_ulBufferIndex;

	bool m_IsTrash;

};

/*
This specialization of the CArray template creates a container object to hold pointers to the SVCICallbackStruct objects.
*/
typedef SVVector< SVCICallbackStruct * > SVCICallbackStructPtrCArray;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCI\SVCICallbackStruct.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:59:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 May 2011 09:50:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the Dalsa/Coreco libraries directly, change the acquisition mode to CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS_WITH_TRASH, and change the buffer allocation to CORBUFFER_VAL_TYPE_SCATTER_GATHER.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Nov 2010 15:38:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Feb 2010 08:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jul 2009 13:04:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new methodology to aquire a buffer from SVObserver and copy/convert data from the acquisition buffer into the SVObserver buffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/