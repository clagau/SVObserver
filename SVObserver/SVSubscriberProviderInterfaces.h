//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSubscriberProviderInterfaces
//* .File Name       : $Workfile:   SVSubscriberProviderInterfaces.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:15:36  $
//******************************************************************************

#ifndef SVSUBSCRIBERPROVIDERINTERFACES_H
#define SVSUBSCRIBERPROVIDERINTERFACES_H

#include "SVOLibrary/SVOCallbackClass.h"

// temp hack
//typedef HRESULT (*SVSUBSCRIBERCALLBACKFNPTR) (ULONG);

class SVSubscriberInterface
{
public:
	// temp hack
	virtual SVOCallbackPtr GetCallbackFunction(const CString& sType) const = 0;

};

class SVProviderInterface
{
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVSubscriberProviderInterfaces.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:15:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 12:54:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2004 07:46:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration; provide interfaces for subscribing to and providing asynchronous events
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
