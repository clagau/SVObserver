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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

/*
This object holds the data response information to a event.
*/
class SVODataResponseClass  //@TODO[Arvid] this is similar to SVOResponseClass: combine or derive?
{
public:
	SVODataResponseClass();
	SVODataResponseClass(const SVODataResponseClass &rRequest);

	~SVODataResponseClass();

	HRESULT Reset();

	bool IsValid() const;
	bool IsComplete() const;

	HRESULT GetIsValid( bool &rbIsValid ) const;
	HRESULT SetIsValid( bool bIsValid );

	HRESULT GetIsComplete( bool &rbIsComplete ) const;
	HRESULT SetIsComplete( bool bIsComplete );

	HRESULT GetOwner( void **pvOwner ) const;
	HRESULT SetOwner( void *pvOwner );

	HRESULT GetStartTick( SVClock::SVTimeStamp& p_rTick ) const;
	HRESULT SetStartTick( const SVClock::SVTimeStamp& p_rTick );

	HRESULT GetEndTick( SVClock::SVTimeStamp& p_rTick ) const;
	HRESULT SetEndTick( const SVClock::SVTimeStamp& p_rTick );




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

};

