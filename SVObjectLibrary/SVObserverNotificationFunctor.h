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

