//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferInterface
//* .File Name       : $Workfile:   SVImageBufferInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 20:14:46  $
//******************************************************************************

#ifndef SVIMAGEBUFFERINTERFACE_H
#define SVIMAGEBUFFERINTERFACE_H

#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVImageBufferHandleInterface.h"

struct SVImageBufferInterface  
{
	SVImageBufferInterface();
	SVImageBufferInterface( const SVImageBufferInterface& p_rObject );
	SVImageBufferInterface( SVSmartHandlePointer p_ImageHandle, const SVDataManagerHandle& p_rIndexHandle );
	virtual ~SVImageBufferInterface();

	const SVImageBufferInterface& operator=( const SVImageBufferInterface& p_rObject );

	virtual void clear();

	virtual HRESULT Assign( SVSmartHandlePointer p_ImageHandle, const SVDataManagerHandle& p_rIndexHandle );

	virtual unsigned char* GetBufferAddress() const;

	virtual const SVClock::SVTimeStamp& GetStartFrameTimeStamp() const;
	virtual void SetStartFrameTimeStamp();
	virtual void SetStartFrameTimeStamp( const SVClock::SVTimeStamp& p_rTimeStamp );

	virtual const SVClock::SVTimeStamp& GetEndFrameTimeStamp() const;
	virtual void SetEndFrameTimeStamp();
	virtual void SetEndFrameTimeStamp( const SVClock::SVTimeStamp& p_rTimeStamp );

	SVSmartHandlePointer m_ImageHandle;
	SVDataManagerHandle m_IndexHandle;

	SVClock::SVTimeStamp m_StartTimeStamp;
	SVClock::SVTimeStamp m_EndTimeStamp;

};

typedef SVSharedPtr< SVImageBufferInterface > SVImageBufferPointer;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVImageLibrary\SVImageBufferInterface.h_v  $
 * 
 *    Rev 1.1   07 May 2013 20:14:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Apr 2013 12:34:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 10:53:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Sep 2009 08:49:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
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

