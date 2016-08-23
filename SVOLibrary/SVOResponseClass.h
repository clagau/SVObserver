// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOResponseClass
// * .File Name       : $Workfile:   SVOResponseClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:20  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <boost/any.hpp>
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVTimerLibrary/SVClock.h"

class SVOResponseClass  
{
public:
	SVOResponseClass();
	SVOResponseClass(const SVOResponseClass &rResponse);
	virtual ~SVOResponseClass();

	const SVOResponseClass &operator=(const SVOResponseClass &rResponse);

	virtual HRESULT Reset();

	virtual bool IsValid() const;
	virtual bool IsComplete() const;

	virtual HRESULT GetIsValid( bool &rbIsValid ) const;
	virtual HRESULT SetIsValid( bool bIsValid );

	virtual HRESULT GetIsComplete( bool &rbIsComplete );
	virtual HRESULT SetIsComplete( bool bIsComplete );

	virtual HRESULT GetOwner( void **pvOwner );
	virtual HRESULT SetOwner( void *pvOwner );

	virtual HRESULT GetStartTick( SVClock::SVTimeStamp& p_rTick );
	virtual HRESULT SetStartTick( const SVClock::SVTimeStamp& p_rTick );

	virtual HRESULT GetEndTick( SVClock::SVTimeStamp& p_rTick );
	virtual HRESULT SetEndTick( const SVClock::SVTimeStamp& p_rTick );

	virtual HRESULT GetExtraData( boost::any& p_rExtraData );
	virtual HRESULT SetExtraData( const boost::any& p_rExtraData );

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
	boost::any mExtraData;  //used by SVCameraTriggerClass::TriggerCallback() only

};

