// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVODataResponseClass
// * .File Name       : $Workfile:   SVODataResponseClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:06  $
// ******************************************************************************

#ifndef SVODATARESPONSECLASS_H
#define SVODATARESPONSECLASS_H

#include <comdef.h>
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVTimerLibrary/SVClock.h"

/*
This object holds the data response information to a event.
*/
class SVODataResponseClass
{
public:
	SVODataResponseClass();
	SVODataResponseClass(const SVODataResponseClass &rRequest);

	virtual ~SVODataResponseClass();

	virtual HRESULT Reset();

	virtual bool IsValid() const;
	virtual bool IsComplete() const;

	virtual HRESULT GetIsValid( bool &rbIsValid ) const;
	virtual HRESULT SetIsValid( bool bIsValid );

	virtual HRESULT GetIsComplete( bool &rbIsComplete ) const;
	virtual HRESULT SetIsComplete( bool bIsComplete );

	virtual HRESULT GetOwner( void **pvOwner ) const;
	virtual HRESULT SetOwner( void *pvOwner );

	virtual HRESULT GetStartTick( SVClock::SVTimeStamp& p_rTick ) const;
	virtual HRESULT SetStartTick( const SVClock::SVTimeStamp& p_rTick );

	virtual HRESULT GetEndTick( SVClock::SVTimeStamp& p_rTick ) const;
	virtual HRESULT SetEndTick( const SVClock::SVTimeStamp& p_rTick );

	virtual HRESULT GetExtraData( unsigned long &ulData) const;
	virtual HRESULT SetExtraData( unsigned long ulData );

	//This attribute holds the Data Manager index of the associated data element.
	SVDataManagerHandle mDMHandle;

private:
	//This attribute holds a pointer to the owner of the response.
	bool mbIsValid;
	//This attribute holds a pointer to the parent of the response. 
	bool mbIsComplete;

	//This attribute holds a pointer to the owner of the response.
	void *mpvOwner;

	//This attribute holds the start time stamp the response.
	SVClock::SVTimeStamp m_StartTick;
	//This attribute holds the end time stamp the response.
	SVClock::SVTimeStamp m_EndTick;

	//This attribute holds context data of the response. 
	unsigned long mulExtraData;

};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVODataResponseClass.h_v  $
 * 
 *    Rev 1.1   08 May 2013 15:28:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Apr 2013 13:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 11:55:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2011 14:49:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new device management classes to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
