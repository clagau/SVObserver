//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionBufferInterface
//* .File Name       : $Workfile:   SVAcquisitionBufferInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 20:14:40  $
//******************************************************************************

#ifndef SVACQUISITIONBUFFERINTERFACE_H
#define SVACQUISITIONBUFFERINTERFACE_H

#include "SVTimerLibrary/SVClock.h"
#include "SVImageBufferInterface.h"

class SVAcquisitionBufferInterface  
{
public:
	virtual ~SVAcquisitionBufferInterface();

	virtual SVClock::SVTimeStamp GetTimeStamp() const = 0;
	
	virtual size_t GetBufferWidth() const = 0;
	virtual size_t GetBufferHeight() const = 0;
	virtual int GetBufferFormat() const = 0;

	virtual HRESULT GetNextBuffer( SVImageBufferInterface& p_rBuffer ) = 0;
	virtual HRESULT UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer ) = 0;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVImageLibrary\SVAcquisitionBufferInterface.h_v  $
 * 
 *    Rev 1.1   07 May 2013 20:14:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Apr 2013 12:34:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 10:37:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Jul 2009 13:26:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image library to include the new data manager objects to track the indexes.  Updated DLL interface to use the new buffer transfer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
