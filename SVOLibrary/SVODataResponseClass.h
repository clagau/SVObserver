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

