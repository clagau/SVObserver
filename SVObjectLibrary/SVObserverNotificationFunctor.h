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

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <memory>
#pragma endregion Includes

class SVObserverNotificationFunctor
{
public:
	virtual ~SVObserverNotificationFunctor();

protected:
	SVObserverNotificationFunctor();

};

typedef std::shared_ptr< SVObserverNotificationFunctor > SVObserverNotificationFunctorPtr;

