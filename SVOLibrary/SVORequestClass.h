// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVORequestClass
// * .File Name       : $Workfile:   SVORequestClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:14  $
// ******************************************************************************

#ifndef SVOREQUESTCLASS_H
#define SVOREQUESTCLASS_H

#include <comdef.h>
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVOResponseClass.h"

/*
This object holds the request and response information to a event transaction.
*/
class SVORequestClass
{
public:
	SVORequestClass();
	SVORequestClass(const SVORequestClass &rRequest);

	virtual ~SVORequestClass();

	const SVORequestClass &operator=(const SVORequestClass &rRequest);

	virtual HRESULT Reset();

	virtual bool IsValid() const;
	virtual bool IsComplete() const;

	virtual HRESULT GetIsValid( bool &rbIsValid ) const;
	virtual HRESULT SetIsValid( bool bIsValid );

	virtual HRESULT GetIsComplete( bool &rbIsComplete );
	virtual HRESULT SetIsComplete( bool bIsComplete );

	virtual HRESULT GetOwner( void **pvOwner );
	virtual HRESULT SetOwner( void *pvOwner );

	virtual HRESULT GetParent( void **pvParent );
	virtual HRESULT SetParent( void *pvParent );

	virtual HRESULT GetExtraData( void **ppvResponse );
	virtual HRESULT SetExtraData( void *pResponse );

	virtual HRESULT GetStartTick( SVClock::SVTimeStamp& p_rTick );
	virtual HRESULT SetStartTick( const SVClock::SVTimeStamp& p_rTick );

	virtual HRESULT GetResponse( SVOResponseClass& p_rResponse );
	virtual HRESULT SetResponse( SVOResponseClass p_rResponse );

	//This attribute holds the Data Manager index of the associated data element.
	SVDataManagerHandle mDMHandle;

private:

	//This attribute holds a pointer to the owner of the request.
	void *mpvOwner;
	//This attribute holds a pointer to the parent of the request. 
	void *mpvParent;
	//This attribute holds a pointer to context data of the request. 
	void *mpvExtraData;

	//This attribute holds the validity state of the request.
	bool mbIsValid;
	//This attribute holds the completion state of the request.
	bool mbIsComplete;

	//This attribute holds the start time stamp the request.
	SVClock::SVTimeStamp m_StartTick;

	//This attribute holds the response data associated with this request.
	SVOResponseClass mResponse;

};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVORequestClass.h_v  $
 * 
 *    Rev 1.1   08 May 2013 15:28:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Apr 2013 13:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 11:57:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Feb 2010 10:58:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Sep 2009 09:24:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jul 2009 10:40:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the code to handle buffer transfer and signal handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jan 2003 16:18:52   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new Data Manager index attribute to classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:17:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
