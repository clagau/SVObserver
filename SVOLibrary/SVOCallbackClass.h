// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCallbackClass
// * .File Name       : $Workfile:   SVOCallbackClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:54:10  $
// ******************************************************************************

#ifndef SVOCALLBACKCLASS_H
#define SVOCALLBACKCLASS_H

#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVORequestClass.h"
#include "SVQueueObject.h"

typedef HRESULT ( CALLBACK *SVOCallbackPtr )( void *,   /* Owner */
                                              void *,   /* Caller */ 
                                              void * ); /* Data */ 

/*
This class is used to identify the callback requesters and holds the data to be requested and returned.
*/
class SVOCallbackClass  
{
public:
	typedef SVTQueueObject< SVORequestClass > SVRequestQueue;
	typedef SVTQueueObject< SVOResponseClass > SVResponseQueue;

	SVOCallbackClass();
	SVOCallbackClass(const SVOCallbackClass& rData);
	virtual ~SVOCallbackClass();

	const SVOCallbackClass& operator=(const SVOCallbackClass& rData);

	//This attribute holds the function to call when data is available to return.
	SVOCallbackPtr mpCallback;
	//This attribute holds a pointer to the owner/receipent of the callback.
  void *mpvOwner;
	//This attribute holds a pointer to the caller of the callback function.
  void *mpvCaller;

	//This attribute holds the reference count of this object.
	long m_lRefCount;

	//This attribute holds the queue of requested data.
	SVRequestQueue mRequestQueue;
	//This attribute holds the queue of response data.
	SVResponseQueue mResponseQueue;
	
};

typedef SVSharedPtr< SVOCallbackClass > SVOCallbackClassPtr;
typedef SVTQueueObject< SVOCallbackClassPtr > SVCallbackClassPtrQueue;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVOCallbackClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:54:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Mar 2011 15:42:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Feb 2010 10:58:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jul 2009 10:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the code to handle buffer transfer and signal handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Mar 2003 12:18:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added new member variable m_lRefCount.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:17:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
