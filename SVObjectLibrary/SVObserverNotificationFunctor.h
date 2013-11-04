//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObserverNotificationFunctor
//* .File Name       : $Workfile:   SVObserverNotificationFunctor.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:20:22  $
//******************************************************************************

#ifndef SVOBSERVERNOTIFICATIONFUNCTOR_H
#define SVOBSERVERNOTIFICATIONFUNCTOR_H

#include "SVUtilityLibrary/SVSharedPtr.h"

class SVObserverNotificationFunctor
{
public:
	virtual ~SVObserverNotificationFunctor();

protected:
	SVObserverNotificationFunctor();

};

typedef SVSharedPtr< SVObserverNotificationFunctor > SVObserverNotificationFunctorPtr;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObserverNotificationFunctor.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 17:20:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:04:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated object manager to include the new notification functionality to managed object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/